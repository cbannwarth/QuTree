//
// Created by Roman Ellerbrock on 2020-01-17.
//
#include "Core/Tensor_Implementation.h"

typedef complex<double> cd;

// Tensor instantiations
template class Tensor<double>;
template class Tensor<complex<double>>;

template Matrix<cd> HoleProduct<cd>(const Tensor<cd>& A, const Tensor<cd>& B, size_t k);
template Tensor<cd> multAB<cd, cd>(const Matrix<cd>& A, const Tensor<cd>& B, size_t mode);
template void multAB<cd, cd>(Tensor<cd>& C, const Matrix<cd>& A, const Tensor<cd>& B, size_t mode, bool zero);
template Tensor<cd> multATB<cd, cd>(const Matrix<cd>& A, const Tensor<cd>& B, size_t mode);
template Tensor<cd> multStateAB(const Matrix<cd>& A, const Tensor<cd>& B);
template Tensor<cd> multStateArTB<cd, cd>(const Matrix<cd>& A, const Tensor<cd>& B);
template void GramSchmidt<cd>(Tensor<cd>& A);
template void multStateAB<cd, cd>(Tensor<cd>& C, const Matrix<cd>& A, const Tensor<cd>& B, bool zero);
template Tensor<cd> Project<cd>(const Tensor<cd>& A, const Tensor<cd>& B);
template Tensor<cd> ProjectOut<cd>(const Tensor<cd>& A, const Tensor<cd>& B);
template Tensor<cd> ProjectOrthogonal<cd>(const Tensor<cd>& A, const Tensor<cd>& B);
template void multAdd<cd, cd>(Tensor<cd>& A, const Tensor<cd>& B, cd coeff);
template Tensor<cd> conj<cd>(Tensor<cd> A);
template double Residual(Tensorcd A, const Tensorcd& B);

template ostream& operator<< <cd> (ostream&, const Tensor<cd>& );
template istream& operator>> <cd> (istream&, Tensor<cd>& );
template bool operator== <cd>(const Tensor<cd>& A, const Tensor<cd>& B);

typedef double doub;
template Matrix<double> HoleProduct<double>(const Tensor<double>& A, const Tensor<double>& B, size_t k);
template Tensor<double> multAB<doub, doub>(const Matrix<double>& A, const Tensor<double>& B, size_t mode);
template void multAB<doub, doub>(Tensor<double>& C, const Matrix<double>& A, const Tensor<double>& B, size_t mode, bool zero);
template Tensor<double> multATB<doub, doub>(const Matrix<double>& A, const Tensor<double>& B, size_t mode);
template Tensor<double> multStateAB(const Matrix<double>& A, const Tensor<double>& B);
template Tensor<double> multStateArTB<doub, doub>(const Matrix<double>& A, const Tensor<double>& B);
template void GramSchmidt<double>(Tensor<double>& A);
template void multStateAB<doub, doub>(Tensor<double>& C, const Matrix<double>& A, const Tensor<double>& B, bool zero);
template void multAdd<doub, doub>(Tensor<double>& A, const Tensor<double>& B, doub coeff);
template Tensor<double> conj<double>(Tensor<double> A);
template double Residual(Tensord A, const Tensord& B);

template ostream& operator<< <doub> (ostream&, const Tensor<doub>& );
template istream& operator>> <doub> (istream&, Tensor<doub>& );
template bool operator== <doub>(const Tensor<doub>& A, const Tensor<doub>& B);

/// Mixed double/complex<double>
template void multAdd<cd, double>(Tensor<cd>& A, const Tensor<cd>& B, double coeff);
template Tensor<cd> multAB<cd, doub>(const Matrix<doub>& A, const Tensor<cd>& B, size_t mode);
template Tensor<cd> multATB<cd, doub>(const Matrix<doub>& A, const Tensor<cd>& B, size_t mode);

