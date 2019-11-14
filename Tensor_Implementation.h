#pragma once
#include "Tensor.h"
#include "TensorDim_Extension.h"

//////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////

template<typename T>
inline T& Tensor<T>::operator()(const size_t i) const {
	size_t dimtot = dim.getdimtot();
	assert(i >= 0 && i < dimtot);
	return coeffs[i];
}

template<typename T>
inline T& Tensor<T>::operator()(const size_t i) {
	size_t dimtot = dim.getdimtot();
	assert(i >= 0 && i < dimtot);
	return coeffs[i];
}

template<typename T>
Tensor<T>::Tensor(const TensorDim& dim_, const bool InitZero)
	:dim(dim_), coeffs(new T[dim_.getdimtot()]) {
	if (InitZero) { Zero(); }
}

// Copy constructor
template<typename T>
Tensor<T>::Tensor(const Tensor& old)
	:Tensor(old.dim, false) {
	for (size_t i = 0; i < dim.getdimtot(); i++) {
		coeffs[i] = old.coeffs[i];
	}
}

// Copy-Multyply constructor
template<typename T>
Tensor<T>::Tensor(const Tensor& old, T factor)
	:Tensor(old.dim, false) {
	for (size_t i = 0; i < dim.getdimtot(); i++) {
		coeffs[i] = old.coeffs[i] * factor;
	}
}

// Move constructor
template<typename T>
Tensor<T>::Tensor(Tensor&& old)noexcept
	:dim(old.dim), coeffs(old.coeffs) {
	old.coeffs = nullptr;
}

// Copy Assignment Operator
template<typename T>
Tensor<T>& Tensor<T>::operator=(const Tensor& old) {
	Tensor tmp(old);
	*this = move(tmp);
	return *this;
}

// Move Assignment Operator
template<typename T>
Tensor<T>& Tensor<T>::operator=(Tensor&& old)noexcept {
	delete[] coeffs;
	dim = old.dim;
	coeffs = old.coeffs;
	old.coeffs = nullptr;
	return *this;
}

template<typename T>
Tensor<T>::~Tensor() {
	delete[] coeffs;
}

//////////////////////////////////////////////////////////
// Bracket Operators
//////////////////////////////////////////////////////////
template<typename T>
inline T& Tensor<T>::operator()(const size_t i, const size_t n) const {
	size_t dimpart = dim.getdimpart();
	assert(i < dimpart);
	assert(n < dim.getntensor());
	return coeffs[n * dimpart + i];
}

template<typename T>
inline T& Tensor<T>::operator()(const size_t i, const size_t n) {
	size_t dimpart = dim.getdimpart();
	assert(i < dimpart);
	assert(n < dim.getntensor());
	return coeffs[n * dimpart + i];
}

template<typename T>
inline T& Tensor<T>::operator()(const size_t i, const size_t j, const size_t k, const size_t f, const size_t n) {
	size_t a = dim.Before(f);
	size_t b = dim.Active(f);
	size_t dimpart = dim.getdimpart();
	size_t idx = n * dimpart + k * a * b + j * a + i;
	assert(i < a);
	assert(j < b);
	assert(n < dim.getntensor());
	assert(f < dim.F());
	assert(k < dim.Behind(f));
	return coeffs[idx];
}

template<typename T>
inline T& Tensor<T>::operator()(const size_t i, const size_t j, const size_t k, const size_t f, const size_t n) const {
	size_t a = dim.Before(f);
	size_t b = dim.Active(f);
	size_t dimpart = dim.getdimpart();
	size_t idx = n * dimpart + k * a * b + j * a + i;
	assert(i < a);
	assert(j < b);
	assert(n < dim.getntensor());
	assert(f < dim.F());
	assert(k < dim.Behind(f));
	return coeffs[idx];
}

// Double Hole Operator
template<typename T>
T& Tensor<T>::operator()(const size_t bef, const size_t i, const size_t mid,
	const size_t j, const size_t beh, const size_t mode1, const size_t mode2,
	const size_t n) const {
	assert(mode1 < mode2);
	assert(mode2 < dim.F());
	size_t dimpart = dim.getdimpart();
	size_t before = dim.Before(mode1);
	size_t active1 = dim.Active(mode1);
	size_t active2 = dim.Active(mode2);
	size_t before2 = dim.Before(mode2);
	size_t idx = n * dimpart + before2 * active2 * beh + before2 * j +
		mid * before * active1 + before * i + bef;
	// mostly checking
	assert(bef < before);
	assert(i < active1);
	assert(j < active2);
	assert(beh < dim.Behind(mode2));
	assert(n < dim.getntensor());
	return coeffs[idx];
}

//////////////////////////////////////////////////////////
// File handling
//////////////////////////////////////////////////////////
template<typename T>
void Tensor<T>::print(ostream& os) const {
	for (size_t n = 0; n < dim.getntensor(); n++) {
		for (size_t i = 0; i < dim.getdimpart(); i++)
			os << (*this)(i, n) << " ";
		os << endl;
	}
	os << endl;
}

template<typename T>
void Tensor<T>::WriteRaw(ostream& os) const {
	const char record = (char) 255;
	for (size_t i = 0; i < dim.getdimtot(); i++) {
		os << operator()(i);
		for (size_t j = 0; j < 8; j++)
			os << record;
	}
}

template<typename T>
void Tensor<T>::ReadRaw(istream& is) {
	char record;
	for (size_t i = 0; i < dim.getdimtot(); i++) {
		is >> operator()(i);
		for (size_t j = 0; j < 8; j++)
			is >> record;
	}
}

template<typename T>
void Tensor<T>::Write(ostream& os) const {
	for (size_t i = 0; i < dim.getdimtot(); i++)
		os << operator()(i);
}

template<typename T>
void Tensor<T>::WriteBin(ofstream& os) const {
	// Verification
	os.write("TENS", 4);

	// Write the TensorDim
	dim.WriteBin(os);

	// Write the size
	int32_t size = sizeof(T);
	os.write((char *) &size, sizeof(size));

	// Write the Coefficients
	for (size_t i = 0; i < dim.getdimtot(); i++) {
		T Coeff_now = operator()(i);
		os.write((char *) &Coeff_now, size);
	}
	os.flush();
}

template<typename T>
void Tensor<T>::ReadBin(ifstream& is) {
	// Check if binary string contains a Tensor
	char check[5];
	is.read(check, 4);
	string s_check(check, 4);
	string s_key("TENS");
	assert(s_key == s_check);

	// Read the TensorDim
	TensorDim newtdim;
	newtdim.ReadDim(is);

	// Resize the Tensor
	(*this) = Tensor<T>(newtdim, false);

	// Read the size
	int32_t size;
	is.read((char *) &size, sizeof(size));
	assert(size == sizeof(T));

	// Read the coefficients

	for (size_t i = 0; i < dim.getdimtot(); i++) {
		T Coeff_now;
		is.read((char *) &Coeff_now, size);
		operator()(i) = Coeff_now;
	}
}

template<typename T>
void Tensor<T>::Read(istream& os) {
	for (size_t i = 0; i < dim.getdimtot(); i++)
		os >> operator()(i);
}

template<typename T>
void Tensor<T>::ReadFortranBinary(int id) {
	// Read a block of tensor components in Fortran style,
	// i.e. after each component there is an 8 byte record
	int dimension = dim.getdimtot();
	freadcomplexfortran_(&id, (complex<double> *) coeffs, &dimension);
}

//////////////////////////////////////////////////////////
// Math Operators
//////////////////////////////////////////////////////////

template<typename T>
void Tensor<T>::operator+=(const Tensor& A) {
	assert(A.Dim().getdimtot() == Dim().getdimtot());
	for (size_t i = 0; i < A.Dim().getdimtot(); i++) {
		(*this)(i) += A(i);
	}
}

template<typename T>
void Tensor<T>::operator*=(T a) {
	for (size_t i = 0; i < Dim().getdimtot(); i++) {
		operator()(i) = a * operator()(i);
	}
}

template<typename T>
void Tensor<T>::operator/=(T a) {
	for (size_t i = 0; i < Dim().getdimtot(); i++) {
		operator()(i) = operator()(i) / a;
	}
}

template<typename T>
Tensor<T> Tensor<T>::coeffprod(const Tensor<T>& A, const Tensor<T>& B) {
	assert(A.Dim().getdimtot() == B.Dim().getdimtot());
	Tensor C(A.Dim());
	for (size_t i = 0; i < A.Dim().getdimtot(); i++) {
		C(i) = A(i) * B(i);
	}
	return C;
}

//////////////////////////////////////////////////////////
// Adjust Dimensions
//////////////////////////////////////////////////////////
template<typename T>
Tensor<T> Tensor<T>::AdjustDimensions(const TensorDim& newTDim)const {
	// Increase the dimensions of the Tensor from old TensorDim
	// to new TensorDim 

	assert(newTDim.F() == dim.F());
	// Increase the active modes
	Tensor<T> Acoeff(*this);
	for (size_t k = 0; k < dim.F(); k++) {
		size_t act = newTDim.Active(k);
		Acoeff = Acoeff.AdjustActiveDim(act, k);
	}

	// Increase the number of Tensors
	size_t ntens = newTDim.getntensor();
	Acoeff = Acoeff.AdjustStateDim(ntens);

	return Acoeff;
}

template<typename T>
Tensor<T> Tensor<T>::AdjustActiveDim(size_t active, size_t mode)const {
	// Adjust the active dimension in the coordinate "mode".
	// If the new active is smaller, the norm of the tensors is
	// not conserved.

	assert(mode < dim.F());
	assert(active > 0);

	// Create a new Tensor with the adjusted dim
	vector<size_t> dimlist = dim.getdimlist();
	dimlist[mode] = active;
	size_t ntensor = dim.getntensor();
	TensorDim newTDim(dimlist, ntensor);
	Tensor<T> newT(newTDim);

	// Copy the coefficients
	size_t before = dim.Before(mode);
	size_t behind = dim.Behind(mode);
	size_t minactive = min(active, dim.Active(mode));
	for (size_t n = 0; n < ntensor; n++) {
		for (size_t l = 0; l < behind; l++) {
			for (size_t j = 0; j < minactive; j++) {
				for (size_t i = 0; i < before; i++) {
					newT(i, j, l, mode, n) = operator()(i, j, l, mode, n);
				}
			}
		}
	}
	return newT;
}

// Adjust the size of Tensor 
template<typename T>
Tensor<T> Tensor<T>::AdjustStateDim(size_t n)const {
	// Returns a new tensor with n (>=ntensor) tensors
	// The new tensors are all set to zero

	// Create a new TensorDim with the new size
	vector<size_t> dimlist = dim.getdimlist();
	TensorDim newTDim(dimlist, n);
	Tensor<T> newTensor(newTDim);

	// Copy the coefficients
	size_t ntensor = dim.getntensor();
	size_t dimpart = dim.getdimpart();
	size_t ntensmax = min(n, ntensor);
	for (size_t m = 0; m < ntensmax; m++) {
		for (size_t i = 0; i < dimpart; i++) {
			newTensor(i, m) = operator()(i, m);
		}
	}
	return newTensor;
}

//////////////////////////////////////////////////////////
// Operations on Tensors
//////////////////////////////////////////////////////////
template<typename T>
T Tensor<T>::singleDotProduct(const Tensor& A, size_t n, size_t m) const {
	T result = 0;
#pragma omp parallel for reduction(+: result)
	for (size_t i = 0; i < A.Dim().getdimpart(); i++) {
		result += conjugate(operator()(i, n)) * A(i, m);
	}
	return result;
}

template<typename T>
Matrixcd Tensor<T>::DotProduct(const Tensor<T>& A) const {
	TensorDim tdima(A.Dim());
	// Every tensor can have different amount of states but same dimpart

	size_t nmax = tdima.getntensor();
	size_t mmax = dim.getntensor();
	size_t npart = dim.getdimpart();
	assert(tdima.getdimpart() == npart);

	Matrixcd S(mmax, nmax);
#pragma omp parallel for
	for (size_t n = 0; n < nmax; n++) {
		for (size_t m = 0; m < mmax; m++) {
			for (size_t i = 0; i < npart; i++) {
//				S(m, n) += conj(operator()(i, m))*A(i, n);
				S(m, n) += conj(operator[](m * npart + i)) * A[n * npart + i];
			}
		}
	}
	return S;
}

template<typename T>
void Tensor<T>::Zero() {
	for (size_t i = 0; i < dim.getdimtot(); i++)
		coeffs[i] = 0;
}

//////////////////////////////////////////////////////////
// Non-member functions
//////////////////////////////////////////////////////////
template<typename T>
T SingleDotProd(const Tensor<T>& A, const Tensor<T>& B, size_t n, size_t m) {
	TensorDim tdima(A.Dim());
	TensorDim tdimb(B.Dim());

	size_t nmax = tdima.getntensor();
	size_t mmax = tdimb.getntensor();
	size_t npart = tdima.getdimpart();

	// Every tensor can have different amount of states but same dimpart
	assert(npart == tdimb.getdimpart());
	assert(n < nmax);
	assert(m < mmax);

	T result = 0;
#pragma omp parallel for reduction(+:result)
	for (size_t i = 0; i < npart; i++) {
		result += conj(A(i, n)) * B(i, m);
	}
	return result;
}

template<typename T>
void TensorHoleProduct(Matrix<T>& S, const Tensor<T>& A, const Tensor<T>& B,
	size_t before, size_t active1, size_t active2, size_t behind) {
	// Variables for precalculation of indices
	size_t actbef1 = active1 * before;
	size_t actbef2 = active2 * before;
	size_t Sidx = 0;
	size_t Aidx = 0;
	size_t Bidx = 0;
	size_t ipreidx = 0;
	size_t jpreidx = 0;
	size_t npreidx1 = 0;
	size_t npreidx2 = 0;

//#pragma omp parallel for
	for (size_t n = 0; n < behind; n++) {
		npreidx1 = n * actbef1;
		npreidx2 = n * actbef2;
		for (size_t j = 0; j < active2; j++) {
			jpreidx = npreidx2 + j * before;
			for (size_t i = 0; i < active1; i++) {
				// S(i, j)
				Sidx = j * active1 + i;
				ipreidx = npreidx1 + i * before;
				for (size_t l = 0; l < before; l++) {
					// A(l, i, n)
					Aidx = ipreidx + l;
					// B(l, j, n)
					Bidx = jpreidx + l;
					S[Sidx] += conj(A[Aidx]) * B[Bidx];
				}
			}
		}
	}
}

template<typename T>
Matrix<T> HoleProduct(const Tensor<T>& A, const Tensor<T>& B, size_t k) {
	//
	const TensorDim& tdim_a(A.Dim());
	const TensorDim& tdim_b(A.Dim());

	size_t nstates = tdim_a.getntensor();
	size_t active1 = tdim_a.Active(k);
	size_t before = tdim_a.Before(k);
	size_t behind = tdim_a.Behind(k) * nstates;
	size_t active2 = tdim_b.Active(k);
	assert(tdim_a.getdimtot() / active1 == tdim_b.getdimtot() / active2);

	Matrix<T> S(active1, active2);

	TensorHoleProduct(S, A, B, before, active1, active2, behind);

	return S;
}

template<typename T, typename U>
void mattensor(Tensor<T>& C, const Matrix<U>& A, const Tensor<T>& B,
	size_t before, size_t activeC, size_t activeB, size_t behind, bool zero) {
	// Null the result tensor if flag is set to "true"
	if (zero) { C.Zero(); }

	// Variables to Precompute index values
	size_t actbefB = activeB * before;
	size_t actbefC = activeC * before;
	size_t Aidx = 0;
	size_t Bidx = 0;
	size_t Cidx = 0;
	size_t kpreidxB = 0;
	size_t kpreidxC = 0;
	size_t lpreidx = 0;
	size_t jpreidx = 0;
	size_t lactive = 0;

	if (before == 1) {
//		#pragma omp for private(kpreidx, Bidx, Cidx, Aidx)
		for (size_t k = 0; k < behind; ++k) {
			kpreidxB = k * actbefB;
			kpreidxC = k * actbefC;
			for (size_t l = 0; l < activeB; ++l) {
				Bidx = l + kpreidxB;
				for (size_t j = 0; j < activeC; ++j) {
					Cidx = j + kpreidxC;
					Aidx = l * activeC + j;
					C[Cidx] += A[Aidx] * B[Bidx];
				}
			}
		}
	} else {
//#pragma omp parallel for private(Aidx, Bidx, Cidx, kpreidxB, kpreidxC, lpreidx, lactive, jpreidx)
		for (size_t k = 0; k < behind; ++k) {
			kpreidxB = k * actbefB;
			kpreidxC = k * actbefC;
			for (size_t l = 0; l < activeB; ++l) {
				lpreidx = l * before + kpreidxB;
				lactive = l * activeC;
				for (size_t j = 0; j < activeC; ++j) {
					Aidx = lactive + j;
					jpreidx = j * before + kpreidxC;
					for (size_t i = 0; i < before; ++i) {
						Cidx = jpreidx + i;
						Bidx = lpreidx + i;
						C[Cidx] += A[Aidx] * B[Bidx];
					}
				}
			}
		}
	}
}

template<typename T, typename U>
void Tmattensor(Tensor<T>& C, const Matrix<U>& A, const Tensor<T>& B,
	size_t before, size_t active1, size_t active2, size_t behind, bool zero) {
	// Null the result tensor if flag is set to "true"
	if (zero) { C.Zero(); }

	size_t actbef1 = active1 * before;
	size_t actbef2 = active2 * before;
	size_t Cidx = 0;
	size_t Bidx = 0;
	size_t Aidx = 0;
	size_t kpreidx1 = 0;
	size_t kpreidx2 = 0;
	size_t lpreidx = 0;
	size_t jpreidx = 0;

	if (before == 1) {
//#pragma omp parallel for
		for (size_t k = 0; k < behind; ++k) {
			kpreidx1 = k * actbef1;
			kpreidx2 = k * actbef2;
			for (size_t l = 0; l < active1; ++l) {
				Bidx = l + kpreidx1;
				for (size_t j = 0; j < active2; ++j) {
					Cidx = j + kpreidx2;
					Aidx = j * active1 + l;
					C[Cidx] += conj(A[Aidx]) * B[Bidx];
//					C[Cidx] += conj(A[l, j]) * B[Bidx];
				}
			}
		}
	} else {
//#pragma omp parallel for
		for (size_t k = 0; k < behind; ++k) {
			kpreidx1 = k * actbef1;
			kpreidx2 = k * actbef2;
			for (size_t l = 0; l < active1; ++l) {
				lpreidx = l * before + kpreidx1;
				for (size_t j = 0; j < active2; ++j) {
					Aidx = j * active1 + l;
					jpreidx = j * before + kpreidx2;
					for (size_t i = 0; i < before; ++i) {
						Cidx = jpreidx + i;
						Bidx = lpreidx + i;
						C[Cidx] += conj(A[Aidx]) * B[Bidx];
//						C[Cidx] += conj(A[l, j]) * B[Bidx];
					}
				}
			}
		}
	}
}

template<typename T, typename U>
void multAB(Tensor<T>& C, const Matrix<U>& A, const Tensor<T>& B, size_t mode, bool zero) {
	TensorDim tdim(B.Dim());
	TensorDim tdimC(C.Dim());

	size_t behind = tdim.Behind(mode) * tdim.getntensor();
	size_t before = tdim.Before(mode);
	size_t active1 = A.Dim1();
	size_t active2 = A.Dim2();

	assert(mode < tdim.F());
	assert(A.Dim2() == tdim.Active(mode));
	assert(A.Dim1() == tdimC.Active(mode));

	mattensor(C, A, B, before, active1, active2, behind, zero);
}

template<typename T, typename U>
Tensor<T> multAB(const Matrix<U>& A, const Tensor<T>& B, size_t mode) {
	const TensorDim& tdim(B.Dim());
	assert(mode < tdim.F());
	assert(mode >= 0);
	assert(A.Dim1() == B.Dim().Active(mode));

	if (A.Dim1() == A.Dim2()) {
		Tensor<T> C(tdim);
		size_t behind = tdim.Behind(mode) * tdim.getntensor();
		size_t active = tdim.Active(mode);
		size_t before = tdim.Before(mode);
		mattensor(C, A, B, before, active, active, behind, false);
		return C;
	} else {
		TensorDim tdim(B.Dim());
		size_t active1 = A.Dim1();
		tdim = TensorDim_Extension::ReplaceActive(tdim, mode, active1);
		Tensor<T> C(tdim);
		size_t behind = tdim.Behind(mode) * tdim.getntensor();
		size_t active2 = tdim.Active(mode);
		size_t before = tdim.Before(mode);
		cout << "non-quadratic mattensor implemented but not tested, yet.\n";
		mattensor(C, A, B, before, active1, active2, behind, false);
		getchar();
		return C;
	}
}

template<typename T, typename U>
Tensor<T> multATB(const Matrix<U>& A, const Tensor<T>& B, size_t mode) {
	const TensorDim& tdim(B.Dim());
	assert(mode < tdim.F());
	assert(mode >= 0);
	assert(A.Dim1() == B.Dim().Active(mode));

	if (A.Dim1() == A.Dim2()) {
		Tensor<T> C(tdim);
		size_t behind = tdim.Behind(mode) * tdim.getntensor();
		size_t active = tdim.Active(mode);
		size_t before = tdim.Before(mode);
		Tmattensor(C, A, B, before, active, active, behind, false);
		return C;
	} else {
		TensorDim tdim(B.Dim());
		size_t behind = tdim.Behind(mode) * tdim.getntensor();
		size_t active = tdim.Active(mode);
		size_t before = tdim.Before(mode);
		tdim = TensorDim_Extension::ReplaceActive(tdim, mode, A.Dim2());
		Tensor<T> C(tdim);
		cout << "non-quadratic mattensor implemented but not tested, yet.\n";
		Tmattensor(C, A, B, before, active, A.Dim2(), behind, false);
		getchar();
		return C;
	}
}

template<typename T, typename U>
void multStateAB(Tensor<T>& C, const Matrix<U>& A, const Tensor<T>& B, bool zero) {
	const TensorDim& tdimB(B.Dim());
	const TensorDim& tdimC(C.Dim());

	const size_t before = tdimB.getdimpart();
	const size_t active1 = tdimB.getntensor();
	const size_t active2 = tdimC.getntensor();
	const size_t behind = 1;

	assert(A.Dim2() == active1);
	assert(A.Dim1() == active2);
	assert(before == tdimC.getdimpart());

	mattensor(C, A, B, before, active1, active2, behind, zero);
}

template<typename T, typename U>
Tensor<T> multStateAB(const Matrix<U>& A, const Tensor<T>& B) {
	const TensorDim& tdim_b(B.Dim());
	size_t ntensor = tdim_b.getntensor();
	assert(A.Dim2() == ntensor);

	TensorDim tdim_c(tdim_b);
	tdim_c.setntensor(A.Dim1());
	Tensor<T> C(tdim_c);
	multStateAB(C, A, B);
	return C;
}

template<typename T, typename U>
Tensor<T> multStateArTB(const Matrix<U>& A, const Tensor<T>& B) {
	TensorDim tdim(B.Dim());
	assert(A.Dim1() == A.Dim2());
	assert(A.Dim2() == B.Dim().getntensor());

	Tensor<T> C(tdim);
	for (size_t n = 0; n < tdim.getntensor(); n++)
		for (size_t m = 0; m < tdim.getntensor(); m++)
			for (size_t i = 0; i < tdim.getdimpart(); i++)
				C(i, m) += A(n, m) * B(i, n);

	return C;
}

template<typename T, typename U>
void multAdd(Tensor<T>& A, const Tensor<T>& B, U coeff) {
	const TensorDim& tdim = A.Dim();
	const TensorDim& tdim_2 = A.Dim();
	size_t dimtot = tdim.getdimtot();
	assert(dimtot == tdim_2.getdimtot());
	for (size_t i = 0; i < dimtot; ++i) {
		A(i) += coeff * B(i);
	}
}

template<typename T>
void GramSchmidt(Tensor<T>& A) {
	// @TODO: Fill in auto-refill

	// control parameters
	size_t maxiter = 15;
	double conver = 1e-12;
	double errorconver = 1e-9;

	TensorDim tdim(A.Dim());
	size_t ntensor = tdim.getntensor();
	size_t dimpart = tdim.getdimpart();

	for (size_t n = 0; n < ntensor; n++) {
		size_t iter = 0;
		double accumoverlap = 1.;
		// orthogonalize on all previous ones and then normalize
		while ((accumoverlap > conver) && (iter < maxiter)) {
			iter++;
			accumoverlap = 0;
			for (size_t m = 0; m < n; m++) {
				// orthogonalize
				T overlap = SingleDotProd(A, A, m, n);
				accumoverlap += abs(overlap);
				for (size_t i = 0; i < dimpart; i++) {
					A(i, n) -= overlap * A(i, m);
				}
			}

			// renormalize
			T norm = SingleDotProd(A, A, n, n);
			if (abs(norm) != 0) {
				norm = sqrt(real(norm));
				for (size_t i = 0; i < dimpart; i++) {
					A(i, n) /= norm;
				}
			}
		}
		// Error message
		if (accumoverlap >= errorconver) {
			cout << "Error: No orthogonality in Gram-Schmidt" << endl;
			cout << "Error measurement: " << conver << endl;
			cout << "Present error: " << accumoverlap << endl;
			cout << "Error acceptance: " << errorconver << endl;

			assert(0);
		}
	}
}

//Projects B on A
template<typename T>
Tensor<complex<double> > Project(const Tensor<complex<double> >& A,
	const Tensor<T>& B) {
	//calculates the overlap of A with it self
	Tensor<complex<double> > Aperp(A);
//	GramSchmidt(Aperp);
	const Matrix<complex<double> > overlap = Aperp.DotProduct(Aperp);

	//invert the overlap
	const Matrix<complex<double> > inverse_operlap = overlap.cInv();

	//calculate the scalar product of A and B
	const Matrix<complex<double> > dotproduct = Aperp.DotProduct(B);

	//multiply the scalar product and the inverse_operlap
	const Matrix<complex<double> > product = inverse_operlap * dotproduct;

	return multStateArTB(product, Aperp);
//	return multStateArTB(dotproduct, Aperp);
}

/*! \brief Project B out of A, i.e. Anew = (1-P_B)*A
 *
 * This routine takes a Tensor A and makes it orthogonal to B.
 * It can be written as A_new = (1 - P_B) A, where P_B is the
 * projector onto B.
 */
template<typename T>
Tensor<T> ProjectOut(const Tensor<T>& A,
	const Tensor<T>& B) {
	Tensorcd projector = Project(B, A);
	Tensorcd perp_A(A);
	const TensorDim& tdim = A.Dim();
	for (size_t i = 0; i < tdim.getdimtot(); ++i) {
		perp_A(i) -= projector(i);
	}
	return perp_A;
}

//Projects B on A
template<typename T>
Tensor<complex<double> > ProjectOrthogonal(const Tensor<complex<double> >& A,
	const Tensor<T>& B) {
	// calculate the scalar product of A and B
	const Matrix<complex<double> > dotproduct = A.DotProduct(B);

	return multStateArTB(dotproduct, A);
}

template<typename T>
Tensor<T> conj(Tensor<T> A) {
	for (size_t i = 0; i < A.Dim().getdimtot(); ++i) {
		A[i] = conj(A[i]);
	}
	return A;
}
