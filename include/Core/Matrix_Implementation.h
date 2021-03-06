#pragma once

#include "Matrix.h"

//////////////////////////////////////////////////////////////////////
// Constructors
//////////////////////////////////////////////////////////////////////
template<typename T>
Matrix<T>::Matrix()
	:Matrix(1, 1) {}

template<typename T>
Matrix<T>::Matrix(size_t dim1, size_t dim2)
	:dim1_(dim1), dim2_(dim2),
	 coeffs_(new T[dim1 * dim2]) {
	assert(dim1 > 0);
	assert(dim2 > 0);
	Zero();
}

template<typename T>
Matrix<T>::Matrix(istream& is)
	:Matrix<T>() {
	Read(is);
}

template<typename T>
Matrix<T>::Matrix(const string& filename)
	:Matrix<T>() {
	Read(filename);
}

// Copy constructor
template<typename T>
Matrix<T>::Matrix(const Matrix& old)
	:Matrix(old.dim1_, old.dim2_) {
	for (size_t i = 0; i < dim1_ * dim2_; i++)
		coeffs_[i] = old.coeffs_[i];
}

// Move constructor
template<typename T>
Matrix<T>::Matrix(Matrix&& old) noexcept
	:dim1_(old.dim1_), dim2_(old.dim2_),
	 coeffs_(old.coeffs_) {
	old.coeffs_ = nullptr;
}

// Copy Assignment Operator
template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix& other) {
	//@TODO: copy dims??
	Matrix tmp(other);
	*this = move(tmp);
	return *this;
}

// Move Assignment Operator
template<typename T>
Matrix<T>& Matrix<T>::operator=(Matrix&& other) noexcept {
	//@TODO: copy dims??// seems to be done, but maybe check again
	dim1_ = other.dim1_;
	dim2_ = other.dim2_;
	delete[] coeffs_;
	coeffs_ = other.coeffs_;
	other.coeffs_ = nullptr;
	return *this;
}

template<typename T>
Matrix<T>::~Matrix() {
	delete[] coeffs_;
}

//////////////////////////////////////////////////////////////////////
// Bracket Operators
//////////////////////////////////////////////////////////////////////
template<typename T>
inline T& Matrix<T>::operator()(const size_t i, const size_t j) const {
	assert(i < dim1_);
	assert(j < dim2_);
	return coeffs_[j * dim1_ + i];
}

template<typename T>
inline T& Matrix<T>::operator()(const size_t i, const size_t j) {
	assert(i < dim1_);
	assert(j < dim2_);
	return coeffs_[j * dim1_ + i];
}

//////////////////////////////////////////////////////////////////////
// Getter & Setters
//////////////////////////////////////////////////////////////////////

template<typename T>
Vector<T> Matrix<T>::row(size_t r) const {
	assert(r < dim1_);
	Vector<T> v(dim2_);
	for (size_t c = 0; c < dim2_; ++c) {
		v(c) = operator()(r, c);
	}
	return v;
}

template<typename T>
Vector<T> Matrix<T>::col(size_t c) const {
	assert(c < dim2_);
	Vector<T> v(dim2_);
	for (size_t r = 0; r < dim1_; ++r) {
		v(r) = operator()(r, c);
	}
	return v;
}

//////////////////////////////////////////////////////////////////////
// Fundamental Math operators
//////////////////////////////////////////////////////////////////////
template<typename T>
Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& B) {
	assert(Dim1() == B.Dim1());
	assert(Dim2() == B.Dim2());
	for (size_t j = 0; j < Dim2(); j++)
		for (size_t i = 0; i < Dim1(); i++)
			operator()(i, j) += B(i, j);
	return (*this);
}

template<typename T>
Matrix<T>& Matrix<T>::operator-=(const Matrix<T>& B) {
	assert(Dim1() == B.Dim1());
	assert(Dim2() == B.Dim2());
	for (size_t j = 0; j < Dim2(); j++)
		for (size_t i = 0; i < Dim1(); i++)
			operator()(i, j) -= B(i, j);
	return (*this);
}

template<typename T>
Matrix<T>& Matrix<T>::operator/=(T coeff) noexcept {
	for (size_t i = 0; i < dim1_ * dim2_; ++i) {
		coeffs_[i] /= coeff;
	}
	return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator*=(T coeff) noexcept {
	for (size_t i = 0; i < dim1_ * dim2_; ++i) {
		coeffs_[i] *= coeff;
	}
	return *this;
}

template<typename T>
bool Matrix<T>::operator==(const Matrix<T>& A) const {
	/// This class checks whether two Matrices are precisely equal.
	/// Note: The routine should not be used to check for approximate
	/// equivalence!
	bool result = true;
	if (dim1_ != A.Dim1()) { result = false; }
	if (dim2_ != A.Dim2()) { result = false; }
	for (size_t i = 0; i < dim1_ * dim2_; ++i) {
		if (operator[](i) != A[i]) { result = false; }
	}
	return result;
}

template<typename T>
bool Matrix<T>::operator!=(const Matrix<T>& A) const {
	return !(this->operator==(A));
}

//////////////////////////////////////////////////////////////////////
// More Math operators
//////////////////////////////////////////////////////////////////////

template<typename T>
double Matrix<T>::FrobeniusNorm() const {
	double norm = 0;
	for (size_t i = 0; i < dim2_; ++i) {
		for (size_t j = 0; j < dim1_; ++j) {
			norm += pow(abs(operator()(j, i)), 2);
		}
	}
	return sqrt(norm);
}

template<typename T>
T Matrix<T>::Trace() const {
	assert(dim1_ == dim2_);
	T norm = 0;
	for (size_t i = 0; i < dim1_; i++) {
		norm += operator()(i, i);
	}
	return norm;
}

template<typename T>
Matrix<T> Matrix<T>::Adjoint() const {
	Matrix B(dim2_, dim1_);
	for (size_t i = 0; i < dim2_; i++)
		for (size_t j = 0; j < dim1_; j++)
			B(i, j) = conjugate(operator()(j, i));
	return B;
}

template<typename T>
Matrix<T> Matrix<T>::Transpose() const {
	Matrix B(dim2_, dim1_);
	for (size_t i = 0; i < dim2_; i++)
		for (size_t j = 0; j < dim1_; j++)
			B(i, j) = operator()(j, i);
	return B;
}

template<typename T>
Vector<T> Matrix<T>::diag() const {
	size_t dim = min(dim1_, dim2_);
	Vector<T> d(dim);
	for (size_t i = 0; i < dim; ++i) {
		d(i) = this->operator()(i, i);
	}
	return d;
}

template<typename T>
SpectralDecompositiond Matrix<T>::rDiag() const {
	Matrixd trafo(dim1_, dim2_);
	Vectord ev(dim1_);
	rDiag(trafo, ev);
	return pair<Matrixd, Vectord>(trafo, ev);
}

template<typename T>
void Matrix<T>::rDiag(Matrix<double>& Transformation, Vector<double>& ev) const {
	assert(dim1_ == dim2_);
	assert(ev.Dim() == dim1_);
	Eigen::MatrixXd A = Eigen::Map<Eigen::MatrixXd>((double *) coeffs_, dim1_, dim2_);

	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver;
	solver.compute(A);
	Eigen::MatrixXd vectors = solver.eigenvectors();
	Eigen::VectorXd eigenv = solver.eigenvalues();
	for (size_t i = 0; i < dim1_; i++)
		ev(i) = eigenv(i);
	for (size_t i = 0; i < dim1_; i++)
		for (size_t j = 0; j < dim2_; j++)
			Transformation(j, i) = vectors(j, i);

	// Phase convention
	for (size_t i = 0; i < dim1_; i++) {
		if (Transformation(0, i) < 0) {
			for (size_t j = 0; j < dim1_; j++) {
				Transformation(j, i) *= -1;
			}
		}
	}
}

template<typename T>
SpectralDecompositioncd Matrix<T>::cDiag() const {
	Matrixcd trafo(dim1_, dim2_);
	Vectord ev(dim1_);
	cDiag(trafo, ev);
	return pair<Matrixcd, Vectord>(trafo, ev);
}

template<typename T>
void Matrix<T>::cDiag(Matrix<complex<double>>& Transformation, Vector<double>& ev) const {
	assert(dim1_ == dim2_);
	assert(ev.Dim() == dim1_);
	Eigen::MatrixXcd A = Eigen::Map<Eigen::MatrixXcd>((complex<double> *) coeffs_, dim1_, dim2_);

	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXcd> solver;
	solver.compute(A);
	Eigen::MatrixXcd vectors = solver.eigenvectors();
	Eigen::VectorXd eigenv = solver.eigenvalues();
	for (size_t i = 0; i < dim1_; i++)
		ev(i) = eigenv(i);
	for (size_t i = 0; i < dim1_; i++)
		for (size_t j = 0; j < dim2_; j++)
			Transformation(j, i) = vectors(j, i);

	// Phase convention
	for (size_t i = 0; i < dim1_; i++) {
		if (real(Transformation(0, i)) < 0) {
			for (size_t j = 0; j < dim1_; j++) {
				Transformation(j, i) *= -1;
			}
		}
	}
}

//Inverts a complex matrix
template<typename T>
Matrix<complex<double> > Matrix<T>::cInv() const {
	assert(dim1_ == dim2_);

	//map everything on classes of Eigen
	Eigen::MatrixXcd A = Eigen::Map<Eigen::MatrixXcd>((complex<double> *) coeffs_, dim1_, dim2_);

	//Invert the matrix
	Eigen::MatrixXcd B = A.inverse();

	//map the result back to MCTDH Matrices
	Matrix<complex<double> > Inverse(dim1_, dim2_);
	for (size_t i = 0; i < dim1_; i++)
		for (size_t j = 0; j < dim2_; j++)
			Inverse(j, i) = B(j, i);

	return Inverse;
}

//////////////////////////////////////////////////////////////////////
// I/O
//////////////////////////////////////////////////////////////////////
template<typename T>
void Matrix<T>::print(ostream& os) const {
	for (size_t i = 0; i < dim2_; i++) {
		for (size_t j = 0; j < dim1_; j++) {
			os << (*this)(j, i) << " ";
		}
		os << endl;
	}
	os << endl;
}

template<typename T>
void Matrix<T>::Write(ostream& os) const {
	// Verification
	os.write("MATR", 4);
	os.write((char *) &dim1_, sizeof(dim1_));
	os.write((char *) &dim2_, sizeof(dim2_));

	int32_t size = sizeof(T);
	os.write((char *) &size, sizeof(size));
	for (size_t i = 0; i < dim1_ * dim2_; i++) {
		T Coeff_now = operator[](i);
		os.write((char *) &Coeff_now, size);
	}
	os.flush();
}

template<typename T>
void Matrix<T>::Write(const string& filename) const {
	ofstream os(filename);
	Write(os);
}

template<typename T>
void Matrix<T>::Read(istream& is) {
	char check[5];
	is.read(check, 4);
	string s_check(check, 4);
	string s_key("MATR");
	assert(s_key == s_check);

	// Read dimensions
	is.read((char *) &dim1_, sizeof(dim1_));
	is.read((char *) &dim2_, sizeof(dim2_));
	(*this) = Matrix<T>(dim1_, dim2_);

	// Read the size of type
	int32_t size;
	is.read((char *) &size, sizeof(size));
	assert(size == sizeof(T));

	// Read the coefficients
	for (size_t i = 0; i < dim1_ * dim2_; i++) {
		T Coeff_now;
		is.read((char *) &Coeff_now, size);
		operator[](i) = Coeff_now;
	}
}

template<typename T>
void Matrix<T>::Read(const string& filename) {
	ifstream is(filename);
	Read(is);
}

template<typename T>
void Matrix<T>::Zero() {
	for (size_t i = 0; i < dim1_ * dim2_; i++)
		coeffs_[i] = 0;
}

//////////////////////////////////////////////////////////////////////
// Non-Member functions
//////////////////////////////////////////////////////////////////////

template<typename T, typename U>
Vector<T> multAB(const Matrix<U>& A, const Vector<T>& B) {
	assert(B.Dim() == A.Dim2());

	Vector<T> C(A.Dim1());
	for (size_t i = 0; i < A.Dim1(); i++) {
		for (size_t j = 0; j < A.Dim2(); j++) {
			C(i) += A(i, j) * B(j);
		}
	}
	return C;
}

template<typename T, typename U>
Vector<T> multATB(const Matrix<U>& A, const Vector<T>& B) {
	assert(B.Dim() == A.Dim2());
	Vector<T> C(A.Dim1());
	for (size_t i = 0; i < A.Dim1(); i++) {
		for (size_t j = 0; j < A.Dim2(); j++) {
			C(i) += conj(A(j, i)) * B(j);
		}
	}
	return C;
}

extern "C" {
// subroutine matvec (mulpsi, psi, matrix, a, b, c, add)
// subroutine rhomat (bra,ket,matrix,a,b,c)
void tmatvec_(double *C, double *B, double *mat,
	int *a, int *b, int *c, int *add);
void trmatvec_(double *C, double *B, double *mat,
	int *a, int *b, int *c, int *add);
}

template<typename T>
Matrix<T> multAB(const Matrix<T>& A, const Matrix<T>& B) {
	assert(A.Dim2() == B.Dim1());
	typedef complex<double> cd;
	typedef double d;

	Matrix<T> C(A.Dim1(), B.Dim2());
//  mulpsi(i,j,k) = psi(i,l,k) * matrix(l,j)
//        C(i, j) =   A(i,l,1) *      B(l,j);
//		               (b,a,c) ->(A.Dim1(), A.Dim2(), 1)
	int b = A.Dim1();
	int a = A.Dim2();
	int c = 1;
	int add = false;

	if (is_same<T, cd>::value && (B.Dim1() == B.Dim2())) {
		tmatvec_((double *) &C[0], (double *) &A[0], (double *) &B[0],
			&a, &b, &c, &add);
	} else if (is_same<T, d>::value && (B.Dim1() == B.Dim2())) {
		trmatvec_((double *) &C[0], (double *) &A[0], (double *) &B[0],
			&a, &b, &c, &add);
	} else {
		auto Ae = Eigen::Map<Eigen::Matrix<T,Eigen::Dynamic, Eigen::Dynamic>>((T*) A.Coeffs(), A.Dim1(), A.Dim2());
		auto Be = Eigen::Map<Eigen::Matrix<T,Eigen::Dynamic, Eigen::Dynamic>>((T*) B.Coeffs(), B.Dim1(), B.Dim2());
		auto Ce = Eigen::Map<Eigen::Matrix<T,Eigen::Dynamic, Eigen::Dynamic>>((T*) C.Coeffs(), C.Dim1(), C.Dim2());
		Ce = Ae * Be;
/*		for (size_t j = 0; j < B.Dim2(); j++) {
			for (size_t i = 0; i < A.Dim1(); i++) {
				for (size_t k = 0; k < A.Dim2(); k++) {
					C(i, j) += A(i, k) * B(k, j);
				}
			}
		}*/
	}
	return C;
}

template<typename T>
Matrix<T> multATB(const Matrix<double>& A, const Matrix<T>& B) {
	assert(A.Dim1() == B.Dim1());
	Matrix<T> C(A.Dim2(), B.Dim2());
	for (size_t j = 0; j < B.Dim2(); j++) {
		for (size_t i = 0; i < A.Dim2(); i++) {
			for (size_t k = 0; k < A.Dim1(); k++) {
				// C(i, j) += A(i, k)^T * B(k, j)
				C(i, j) += A(k, i) * B(k, j);
			}
		}
	}
	return C;
}

template<typename T>
Matrix<complex<double>> multATB(const Matrix<complex<double>>& A, const Matrix<T>& B) {
	assert(A.Dim2() == B.Dim1());
	Matrix<T> C(A.Dim1(), B.Dim2());
	for (size_t j = 0; j < B.Dim2(); j++) {
		for (size_t i = 0; i < A.Dim1(); i++) {
			for (size_t k = 0; k < A.Dim2(); k++) {
				C(i, j) += conj(A(k, i)) * B(k, j);
			}
		}
	}
	return C;
}

template<typename T>
Matrix<T> addAB(const Matrix<T>& A, const Matrix<T>& B) {
	assert(A.Dim1() == B.Dim1());
	assert(A.Dim2() == B.Dim2());
	Matrix<T> C(A.Dim1(), B.Dim2());
	for (size_t j = 0; j < A.Dim2(); j++)
		for (size_t i = 0; i < A.Dim1(); i++)
			C(i, j) = A(i, j) + B(i, j);
	return C;
}

template<typename T, typename U>
Matrix<T> multscalar(const U sca, const Matrix<T>& B) {
	Matrix<T> C(B.Dim1(), B.Dim2());
	for (size_t i = 0; i < B.Dim2(); i++)
		for (size_t j = 0; j < B.Dim1(); j++)
			C(j, i) = sca * B(j, i);
	return C;
}

template<typename T>
Matrix<T> substAB(const Matrix<T>& A, const Matrix<T>& B) {
	assert(A.Dim1() == B.Dim1());
	assert(A.Dim2() == B.Dim2());
	Matrix<T> C(A.Dim1(), B.Dim2());
	for (size_t j = 0; j < A.Dim2(); j++)
		for (size_t i = 0; i < A.Dim1(); i++)
			C(i, j) = A(i, j) - B(i, j);
	return C;
}

template<typename T>
Matrix<T> Re(const Matrix<T>& A) {
	for (size_t j = 0; j < A.Dim2(); ++j) {
		for (size_t i = 0; i < A.Dim1(); ++i) {
			A(i, j) = real(A(i, j));
		}
	}
	return A;
}

//////////////////////////////////////////////////////////////////////
/// operator overloadings
//////////////////////////////////////////////////////////////////////

template<typename T, typename U>
Vector<U> operator*(const Matrix<T>& A, const Vector<U>& v) {
	return multAB(A, v);
}

//////////////////////////////////////////////////////////////////////
/// Diagonalization framework
//////////////////////////////////////////////////////////////////////

template<typename T>
void Diagonalize(Matrix<T>& trafo, Vector<double>& ev, const Matrix<T>& B) {
	assert(B.Dim1() == B.Dim2());
	assert(ev.Dim() == B.Dim1());
	typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> EigenMatrix;

	EigenMatrix A = EigenMatrix((T *) B.Coeffs(), B.Dim1(), B.Dim2());
	Eigen::SelfAdjointEigenSolver<EigenMatrix> solver;
	solver.compute(A);
	EigenMatrix vectors = solver.eigenvectors();
	// typedef Eigen::Matrix<U, Eigen::Dynamic, 1> EigenVector; auto in next line should be this template
	auto eigenv = solver.eigenvalues();
	for (size_t i = 0; i < B.Dim1(); i++)
		ev(i) = eigenv(i);
	for (size_t i = 0; i < B.Dim1(); i++)
		for (size_t j = 0; j < B.Dim2(); j++)
			trafo(j, i) = vectors(j, i);

	// Set phase convention
	for (size_t i = 0; i < B.Dim1(); i++) {
		if (real(trafo(0, i)) < 0) {
			for (size_t j = 0; j < B.Dim1(); j++) {
				trafo(j, i) *= -1;
			}
		}
	}
}

SpectralDecompositioncd Diagonalize(const Matrix<complex<double>>& A) {
	return A.cDiag();
}

void Diagonalize(SpectralDecompositioncd& S, const Matrix<complex<double>>& A) {
	A.cDiag(S.first, S.second);
}

SpectralDecompositiond Diagonalize(const Matrix<double>& A) {
	return A.rDiag();
}

void Diagonalize(SpectralDecompositiond& S, const Matrix<double>& A) {
	A.rDiag(S.first, S.second);
}

template<typename T>
Matrix<T> toMatrix(const SpectralDecomposition<T>& X) {
	const auto& mat = X.first;
	const auto& vec = X.second;
	assert(vec.Dim() > 0);
	assert(mat.Dim2() == vec.Dim());
	size_t dim = vec.Dim();
	auto mat2(mat);
	for (size_t i = 0; i < mat2.Dim1(); ++i) {
		for (size_t k = 0; k < mat2.Dim2(); ++k) {
			mat2(i, k) *= vec(k);
		}
	}
	return mat * mat2.Adjoint();
}

template<typename T>
Matrix<T> BuildInverse(const SpectralDecomposition<T>& X, double eps) {
	auto inv_vec = Inverse(X.second, eps);
	return toMatrix<T>({X.first, inv_vec});
}

template<typename T>
SpectralDecomposition<T> sqrt(SpectralDecomposition<T> X) {
	Vectord& lambda = X.second;
	for (size_t i = 0; i < lambda.Dim(); ++i) {
		lambda(i) = sqrt(lambda(i));
	}
	return X;
}

template<typename T>
SpectralDecomposition<T> inverse(SpectralDecomposition<T> X, double eps) {
	X.second = Inverse(X.second, eps);
	return X;
}

template<typename T>
Matrix<T> IdentityMatrix(size_t dim) {
	Matrix<T> I(dim, dim);
	for (size_t i = 0; i < dim; ++i) {
		I(i, i) = 1.;
	}
	return I;
}

Matrixcd IdentityMatrixcd(size_t dim) {
	return IdentityMatrix<complex<double>>(dim);
}

Matrixd IdentityMatrixd(size_t dim) {
	return IdentityMatrix<double>(dim);
}

template<typename T>
Matrix<T> UnitarySimilarityTrafo(const Matrix<T>& A,
	const Matrix<T>& B) {
	// C=B^T*A*B
	assert(A.Dim1() == B.Dim1());
	assert(A.Dim2() == B.Dim2());
	assert(A.Dim1() == A.Dim2());
	Matrix<T> C(multAB(A, B));
	return multATB(B, C);
}

template<typename T>
Vectord Matrix<T>::SolveSLE(const Vectord& b) {
	// Re-organize b to Eigen-format
	Eigen::VectorXd eb(dim1_);
	for (size_t i = 0; i < dim1_; i++)
		eb(i) = b(i);

	// Solve equations
	Eigen::MatrixXd A = Eigen::Map<Eigen::MatrixXd>((double *) coeffs_, dim1_, dim2_);
	Eigen::ColPivHouseholderQR<Eigen::MatrixXd> dec(A);
	Eigen::VectorXd x = dec.solve(eb);

	// Save to QuTree-Vectord
	Vectord x_(dim2_);
	for (size_t i = 0; i < dim2_; i++)
		x_(i) = x(i);
	return x_;
}

template<typename T>
Matrix<T> Regularize(const Matrix<T>& A, double eps) {
	Matrix<T> B(A);
	size_t dim = min(A.Dim1(), A.Dim2());
	for (size_t i = 0; i < dim; i++) {
		B(i, i) += eps * exp(-B(i, i) / eps);
	}
	return B;
}

template<typename T>
Matrix<T> RealSymmetrize(const Matrix<T>& A) {
	assert(A.Dim1() == A.Dim2());
	Matrix<T> Asym(A.Dim1(), A.Dim2());
	for (size_t i = 0; i < A.Dim1(); ++i) {
		for (size_t j = 0; j < A.Dim2(); ++j) {
			Asym(j, i) = (A(j, i) + A(i, j)) / 2.;
		}
	}
	return Asym;
}

template<typename T>
Matrix<T> EuclideanDistance(const Matrix<T>& A) {
	auto G = multATB(A, A);
	Matrix<T> D(G.Dim1(), G.Dim2());
	for (size_t j = 0; j < G.Dim1(); ++j) {
		for (size_t i = 0; i < G.Dim2(); ++i) {
			D(i, j) = G(i, i) + G(j, j) - 2. * G(i, j);
		}
	}
	return D;
}

template<typename T>
double Residual(const Matrix<T>& A, const Matrix<T>& B) {
	Matrix<T> D = A - B;
	return D.FrobeniusNorm();
}

template<typename T>
ostream& operator<<(ostream& os, const Matrix<T>& A) {
	A.Write(os);
	return os;
}

template<typename T>
istream& operator>>(istream& is, Matrix<T>& A) {
	A.Read(is);
	return is;
}

Eigen::MatrixXd toEigen(Matrixd A) {
	Eigen::MatrixXd Aeigen = Eigen::Map<Eigen::MatrixXd>((double *) A.Coeffs(),
		A.Dim1(), A.Dim2());
	return Eigen::MatrixXd(Aeigen);
}

Eigen::MatrixXcd toEigen(Matrixcd A) {
	Eigen::MatrixXcd Aeigen = Eigen::Map<Eigen::MatrixXcd>((complex<double> *)
		A.Coeffs(), A.Dim1(), A.Dim2());
	return Eigen::MatrixXcd(Aeigen);
}

Matrixd toQutree(const Eigen::MatrixXd& A) {
	Matrixd Aqutree(A.rows(), A.cols());
	for (size_t r = 0; r < A.rows(); ++r) {
		for (size_t c = 0; c < A.cols(); ++c) {
			Aqutree(r, c) = A(r, c);
		}
	}
	return Aqutree;
}

Matrixcd toQutree(const Eigen::MatrixXcd& A) {
	Matrixcd Aqutree(A.rows(), A.cols());
	for (size_t r = 0; r < A.rows(); ++r) {
		for (size_t c = 0; c < A.cols(); ++c) {
			Aqutree(r, c) = A(r, c);
		}
	}
	return Aqutree;
}

Matrixcd QR(const Matrixcd& A) {
//	auto Aeigen = toEigen(A);
	using namespace Eigen;
	MatrixXcd Aeigen = Map<MatrixXcd>((complex<double> *) A.Coeffs(),
		A.Dim1(), A.Dim2());
//	auto& QR = Aeigen.householderQr();
//	const Eigen::MatrixXcd& Q = QR.householderQ();
//	return toQutree(Q);
	HouseholderQR<MatrixXcd> qr(Aeigen);
	MatrixXcd thinQ = qr.householderQ() * MatrixXcd::Identity(A.Dim1(), A.Dim2());
	return toQutree(thinQ);
}

SVDcd svd(const Matrixcd& A) {
	using namespace Eigen;
	MatrixXcd Am = Eigen::Map<MatrixXcd>((complex<double> *) &A(0, 0), A.Dim1(), A.Dim2());
	JacobiSVD<MatrixXcd> svd(Am, ComputeThinU | ComputeThinV);
	auto U = toQutree(svd.matrixU());
	auto V = toQutree(svd.matrixV());
	auto sigma = toQutree(svd.singularValues());
	return {U, V, sigma};
}

SVDd svd(const Matrixd& A) {
	using namespace Eigen;
	MatrixXd Am = Eigen::Map<MatrixXd>((double *) &A(0, 0), A.Dim1(), A.Dim2());
	JacobiSVD<MatrixXd> svd(Am, ComputeThinU | ComputeThinV);
	auto U = toQutree(svd.matrixU());
	auto V = toQutree(svd.matrixV());
	auto sigma = toQutree(svd.singularValues());
	return {U, V, sigma};
}

Matrixcd toMatrix(const SVDcd& svd) {
	const auto& U = get<0>(svd);
	auto V = get<1>(svd);
	const auto& sigma = get<2>(svd);
	for (size_t i = 0; i < V.Dim1(); ++i) {
		for (size_t j = 0; j < V.Dim2(); ++j) {
			V(i, j) *= sigma(j);
		}
	}
	return U * V.Adjoint();
}

template<typename T>
Matrix<T> Submatrix(const Matrix<T> A, size_t dim1, size_t dim2) {
	assert(dim1 <= A.Dim1());
	assert(dim2 <= A.Dim2());
	Matrix<T> B(dim1, dim2);
	for (size_t j = 0; j < dim2; ++j) {
		for (size_t i = 0; i < dim1; ++i) {
			B(i, j) = A(i, j);
		}
	}
	return B;
}

