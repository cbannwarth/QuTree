#include "Tensor_Extension.h"

namespace Tensor_Extension {

	tuple<Tensorcd, Matrixcd, Vectord> SVD(const Tensorcd& A) {
		const TensorShape& tdim = A.shape();
		size_t dimpart = tdim.lastBefore();
		size_t ntensor = tdim.lastDimension();

		using namespace Eigen;
		MatrixXcd Am = Eigen::Map<MatrixXcd>((complex<double> *) &A(0), dimpart, ntensor);
		JacobiSVD<MatrixXcd> svd(Am, ComputeThinU | ComputeThinV);

		Tensorcd U(tdim);
		auto u_mat = svd.matrixU();
		for (size_t i = 0; i < dimpart; ++i) {
			for (size_t n = 0; n < ntensor; ++n) {
				U(i, n) = u_mat(i, n);
			}
		}

		auto v_mat = svd.matrixV();
		Matrixcd V(ntensor, ntensor);
		for (size_t i = 0; i < ntensor; ++i) {
			for (size_t n = 0; n < ntensor; ++n) {
				V(i, n) = v_mat(i, n);
			}
		}

		auto sigma_e = svd.singularValues();
		Vectord sigma(ntensor);
		for (size_t i = 0; i < ntensor; ++i) {
			sigma(i) = sigma_e(i);
		}

		return tuple<Tensorcd, Matrixcd, Vectord>(U, V, sigma);
	}

	tuple<Matrixcd, Matrixcd, Vectord> SVD(const Matrixcd& A) {
		size_t dim1 = A.Dim1();
		size_t dim2 = A.Dim2();

		using namespace Eigen;
		MatrixXcd Am = Eigen::Map<MatrixXcd>((complex<double> *) &A(0, 0), dim1, dim2);
		JacobiSVD<MatrixXcd> svd(Am, ComputeThinU | ComputeThinV);

		auto u_mat = svd.matrixU();
		dim1 = u_mat.rows();
		dim2 = u_mat.cols();
		Matrixcd U(dim1, dim2);
		for (size_t i = 0; i < dim1; ++i) {
			for (size_t n = 0; n < dim2; ++n) {
				U(i, n) = u_mat(i, n);
			}
		}

		auto v_mat = svd.matrixV();
		dim1 = v_mat.rows();
		dim2 = v_mat.cols();
		Matrixcd V(dim1, dim2);
		for (size_t i = 0; i < dim1; ++i) {
			for (size_t n = 0; n < dim2; ++n) {
				V(i, n) = v_mat(i, n);
			}
		}

		auto sigma_e = svd.singularValues();
		size_t mdim = min(dim1, dim2);
		Vectord sigma(mdim);
		for (size_t i = 0; i < mdim; ++i) {
			sigma(i) = sigma_e(i);
		}

		return tuple<Matrixcd, Matrixcd, Vectord>(U, V, sigma);
	}

	template<typename T>
	Matrix<T> Map(const Tensor<T>& A) {
		const TensorShape& tdim = A.shape();
		size_t ntensor = tdim.lastDimension();
		size_t dimpart = tdim.lastBefore();
		Matrix<T> M(dimpart, ntensor);
		for (size_t n = 0; n < ntensor; ++n) {
			for (size_t i = 0; i < dimpart; ++i) {
				M(i, n) = A(i, n);
			}
		}
		return M;
	}

	// @TODO:: Add mapping to and from Eigen

	// Randomly occupy Tensors and Matrices
	template<typename T>
	void Generate_normal(T* A, size_t n, mt19937& gen) {
		uniform_real_distribution<double> dist(-1., 1.);
		for (size_t i = 0; i < n; ++i) {
			A[i] = dist(gen);
		}
	}

	template<typename T>
	void Generate(Tensor<T>& A, mt19937& gen) {
		Generate_normal(&A[0], A.shape().totalDimension(), gen);
	}

	template<typename T>
	void Generate(Matrix<T>& A, mt19937& gen) {
		Generate_normal(&A[0], A.Dim1()*A.Dim2(), gen);
	}

	template<typename T>
	void Generate(Vector<T>& A, mt19937& gen) {
		Generate_normal(&A[0], A.Dim(), gen);
	}

/* //////////////////////////////////////////////
 * Extension of the Tensor class
 *
 * The following functions are excluded from the
 * Tensor class to keep it slim.
 */

	template<typename T>
	Tensor<T> Merge(Tensor<T> A, const Tensor<T>& B) {
		// Merge two Tensors into one.
		const TensorShape& tdim1 = A.shape();
		const TensorShape& tdim2 = B.shape();
		size_t ntens1 = tdim1.lastDimension();
		size_t ntens2 = tdim2.lastDimension();
		A = A.AdjustStateDim(ntens1 + ntens2);
		for (size_t n = 0; n < ntens2; ++n) {
			for (size_t i = 0; i < tdim1.lastBefore(); ++i) {
				A(i, ntens1 + n) = B(i, n);
			}
		}
		return A;
	}

	template<typename T>
	void OuterProductAdd(Matrix<T>& M,
		const Tensor<T>& A, const Tensor<T>& B) {
		const TensorShape& tdim = A.shape();
		size_t dimpart = tdim.lastBefore();
		size_t ntensor = tdim.lastDimension();

#pragma omp parallel for
		for (size_t i = 0; i < dimpart; i++) {
			for (size_t j = 0; j < dimpart; j++) {
				for (size_t n = 0; n < ntensor; n++) {
					M(i, j) += A(i, n) * conj(B(j, n));
				}
			}
		}
	}

	template<typename T>
	Matrix<T> OuterProduct(const Tensor<T>& A, const Tensor<T>& B) {

		const TensorShape& tdim = A.shape();
		size_t dimpart = tdim.lastBefore();
		Matrix<T> M(dimpart, dimpart);
		OuterProductAdd(M, A, B);
		return M;
	}

	template<typename T>
	void WeightedOuterProductAdd(Matrix<T>& M, const Tensor<T>& A,
		const Tensor<T>& B, const Matrix<T>& rho) {
		Tensor<T> mA = multStateArTB(rho, A);
		OuterProductAdd(M, mA, B);
	}

	template<typename T>
	Matrix<T> WeightedOuterProduct(const Tensor<T>& A, const Tensor<T>& B,
		const Matrix<T>& m) {
		Tensor<T> mA = multStateAB(m, A);
//		Tensor<T> mB = multStateAB(m, B);
		return OuterProduct(mA, B);
	}

/* //////////////////////////////////////////////
 * Depricated routines for educational purpose
 *
 * The following routines are depricated but kept
 * to allow for a simple understanding of how they
 * work. The main reason for exchanging these routines
 * is the overhead caused by the bracket operators and
 * the lack of optimizing opertunity
 */
	template<typename T, typename U>
	Tensor<T> OldmultAB(const Matrix<U>& A, const Tensor<T>& B, size_t mode) {
		TensorShape tdim(B.shape());
		assert(mode < tdim.order());
		assert(A.Dim1() == A.Dim2());
		assert(A.Dim1() == B.shape().Active(mode));

		Tensor<T> C(tdim);
		for (size_t n = 0; n < tdim.lastDimension(); n++)
			for (size_t k = 0; k < tdim.after(mode)/ tdim.lastDimension(); k++)
				for (size_t l = 0; l < tdim[mode]; l++)
					for (size_t j = 0; j < tdim[mode]; j++)
						for (size_t i = 0; i < tdim.before(mode); i++) {
							C(i, j, k, mode, n) += A(j, l) * B(i, l, k, mode, n);
						}
		return C;
	}

	template<typename T>
	Matrix<T> OldStateAveragedHoleProduct(const Tensor<T>& A, const Tensor<T>& B, size_t k) {
		TensorShape tdim(A.shape());
		// check wether tensordims are equal
		assert(tdim == B.shape());

		size_t nstates = tdim.lastDimension();
		size_t active = tdim[k];
		size_t before = tdim.before(k);
		size_t behind = tdim.after(k)/nstates;
		Matrix<T> S(active, active);

		for (size_t n = 0; n < nstates; n++) {
			for (size_t beh = 0; beh < behind; beh++) {
				for (size_t i = 0; i < active; i++) {
					for (size_t j = 0; j < active; j++) {
						for (size_t bef = 0; bef < before; bef++) {
							S(i, j) += conj(A(bef, i, beh, k, n)) * B(bef, j, beh, k, n);
						}
					}
				}
			}
		}

		// divide by number of states
//	for (size_t j = 0; j < active_; j++)
//		for (size_t i = 0; i < active_; i++)
//			S(i, j) /= nstates;

		return S;
	}

	template<typename T, typename U>
	Tensor<T> OldmultStateAB(const Matrix<U>& A, const Tensor<T>& B) {
		TensorShape tdim(B.shape());
		assert(A.Dim1() == A.Dim2());
		assert(A.Dim2() == B.shape().getntensor());

		Tensor<T> C(tdim);
		for (size_t n = 0; n < tdim.lastDimension(); n++)
			for (size_t m = 0; m < tdim.lastDimension(); m++)
				for (size_t i = 0; i < tdim.lastBefore(); i++)
					C(i, m) += A(m, n) * B(i, n);

		return C;
	}
}
