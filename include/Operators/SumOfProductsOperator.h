#pragma once
#include "MultiLeafOperator.h"

template<typename T>
class SumOfProductsOperator
	/**
	 * \class SumOfProductsOperator
	 * \ingroup Operators
	 * \brief This class represents a sum-of-products operator.
	 *
	 * SOPs are sums of MultiParticleOperators. The class provides
	 * basic arithmetics that allows to perform high-level operations
	 * on operators.
	 */
{
public:
	// Constructor
	SumOfProductsOperator() = default;

	// Destructor
	~SumOfProductsOperator() = default;

	explicit SumOfProductsOperator(const TTBasis& basis) {
		Initialize(basis);
	}

	explicit SumOfProductsOperator(const MPO<T>& M, T c = 1.);

	void Initialize(const TTBasis& basis) {
		coeff_.clear();
		mpos_.clear();
		SpecialInitialize(basis);
	}

	// Get the number of MPOs in the Hamiltonian
	virtual int size() const { return mpos_.size(); }

	// Get a product-operator from the Hamiltonian
	virtual const MPO<T>& operator()(size_t part) const {
		assert(part < mpos_.size());
		return mpos_[part];
	}

	// Get a product-operator from the Hamiltonian
	virtual const MPO<T>& operator[](size_t part) const {
		assert(part < mpos_.size());
		return mpos_[part];
	}

	virtual MPO<T>& operator()(size_t part) {
		assert(part < mpos_.size());
		return mpos_[part];
	}

	// append a new summand
	void push_back(const MPO<T>& M, complex<double> coeff) {
		mpos_.push_back(M);
		coeff_.push_back(coeff);
	}

	complex<double> Coeff(size_t i) const {
		assert(i < coeff_.size());
		return coeff_[i];
	}

//	vector<MPO<T>>::const_iterator begin() const {
	auto begin() const {
		return mpos_.begin();
	}

//	vector<MPO<T>>::const_iterator end() const {
	auto end() const {
		return mpos_.end();
	}

	//////////////////////////////////////////////////////////////////////
	// Operators
	//////////////////////////////////////////////////////////////////////
	// multiply with coefficient
	friend SumOfProductsOperator<T> operator*(T c,
		const SumOfProductsOperator<T>& A);

	friend SumOfProductsOperator<T> operator*(const SumOfProductsOperator<T>& A,
		T c);

	// multiply with Multiparticleoperator
	friend SumOfProductsOperator<T> operator*(const MPO<T>& M,
		const SumOfProductsOperator<T>& A);

	friend SumOfProductsOperator<T> operator*(const SumOfProductsOperator<T>& A,
		const MPO<T>& M);

	// multiply with SoP-Operator
	friend SumOfProductsOperator<T> operator*(const SumOfProductsOperator<T>& A,
		const SumOfProductsOperator<T>& B);

	// add SoP-Operator
	friend SumOfProductsOperator<T> operator+(const SumOfProductsOperator<T>& A,
		const SumOfProductsOperator<T>& B);

protected:
	vector<MPO<T>> mpos_;
	vector<complex<double> > coeff_;

private:
	virtual void SpecialInitialize(const TTBasis& basis) {
		cerr << "Called SpecialInitialize of SOP-base class." << endl;
	}
};

template <typename T>
using SOP = SumOfProductsOperator<T>;

template <typename T>
SOP<T> multAB(const SOP<T>& A, const SOP<T>& B);

