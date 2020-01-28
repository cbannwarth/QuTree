#include "SumOfProductsOperator.h"

template<typename T>
SOP<T>::SumOfProductsOperator(const MPO<T>& M, T c) {
	push_back(M, c);
}

template<typename T>
SOP<T> multAB(const SOP<T>& A, const SOP<T>& B) {
	SOP<T> C;

	for (size_t i = 0; i < A.size(); i++) {
		const MPO<T> Ai = A(i);
		auto acoeff = A.Coeff(i);
		for (size_t j = 0; j < B.size(); j++) {
			const MPO<T> Bj = B(j);
			auto bcoeff = B.Coeff(j);
			auto ccoeff = acoeff * bcoeff;
			C.push_back(Ai * Bj, ccoeff);
		}
	}

	return C;
}

template<typename T>
SOP<T> operator*(T c, const SOP<T>& A) {
	SOP<T> C;
	for (size_t i = 0; i < A.size(); i++) {
		C.push_back(A(i), c * A.Coeff(i));
	}
	return C;
}

template<typename T>
SOP<T> operator*(const SOP<T>& A, T c) {
	return c * A;
}

template<typename T>
SOP<T> operator*(const MPO<T>& M, const SOP<T>& A) {
	SOP<T> C;
	for (size_t i = 0; i < A.size(); i++) {
		MPO<T> MA = M * A(i);
		C.push_back(MA, A.Coeff(i));
	}
	return C;
}

template<typename T>
SOP<T> operator*(const SOP<T>& A,
	const MPO<T>& M) {
	SOP<T> C;
	for (size_t i = 0; i < A.size(); i++) {
		MPO<T> MA = A(i) * M;
		C.push_back(MA, A.Coeff(i));
	}
	return C;
}

template<typename T>
SOP<T> operator*(const SOP<T>& A,
	const SOP<T>& B) {
	SOP<T> C;
	for (size_t i = 0; i < A.size(); i++) {
		const MPO<T> Ai = A(i);
		auto acoeff = A.Coeff(i);
		for (size_t j = 0; j < B.size(); j++) {
			const MPO<T> Bj = B(j);
			auto bcoeff = B.Coeff(j);
			auto ccoeff = acoeff * bcoeff;
			C.push_back(Ai * Bj, ccoeff);
		}
	}
	return C;
}

// add SoP-Operator
template<typename T>
SOP<T> operator+(const SOP<T>& A,
	const SOP<T>& B) {
	SOP<T> C = B;

	for (size_t i = 0; i < A.size(); i++) {
		C.push_back(A(i), A.Coeff(i));
	}

	return C;
}

template class SumOfProductsOperator<complex<double>>;