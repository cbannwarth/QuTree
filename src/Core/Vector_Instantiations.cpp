//
// Created by Roman Ellerbrock on 2020-01-17.
//
#include "Core/Vector_Implementation.h"

typedef complex<double> cd;
typedef double d;

// Vector instantiations
template class Vector<double>;
template class Vector<complex<double>>;

template double Residual(const Vectord& A, const Vectord& B);
template double Residual(const Vectorcd& A, const Vectorcd& B);

template Vector<d> Inverse(Vector<d> A, d eps);
template Vector<cd> Inverse(Vector<cd> A, d eps);
