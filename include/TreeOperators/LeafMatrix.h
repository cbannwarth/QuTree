//
// Created by Roman Ellerbrock on 2020-01-24.
//
#ifndef LEAFMATRIX_H
#define LEAFMATRIX_H
#include "LeafOperator.h"
#include "Core/Matrix.h"

template<typename T>
class LeafMatrix: public LeafOperator<T>
	/**
	 * \class LeafMatrix
	 * \ingroup Operators
	 * \brief This class allows to create LeafOperators from (factor) Matrices.
	 */
{
public:
	LeafMatrix() = default;

	explicit LeafMatrix(Matrix<T> h); // <- can be added if needed

	~LeafMatrix() = default;

	virtual void Apply(const LeafInterface& grid, Tensor<T>& hAcoeff,
		const Tensor<T>& Acoeff) const override;

private:
	Matrix<T> h_;
};

typedef LeafMatrix<complex<double>> LeafMatrixcd;

typedef LeafMatrix<double> LeafMatrixd;

#endif //LEAFMATRIX_H