#pragma once
#include "stdafx.h"

class TensorABC {
// class TensorABC holds contraction of a tensor of n-th order
public:
	TensorABC() {}

	TensorABC(size_t k, vector<size_t> dim);

	inline size_t getbefore() const { return before; }

	inline size_t getactive() const { return active; }

	inline size_t getbehind() const { return behind; }

	inline size_t gettotal() const { return total; }

private:
	size_t before;
	size_t behind;
	size_t active;
	size_t total;
};

/* *
 * \class TensorDim
 * \ingroup QD-lib
 * \brief This class manages the dimensions of a Tensor.
 * 
 * The class manages dimensions and super-index mappings in a n-th
 * order tensor.
 * \image html mctdh++_TensorDim_1.png
 * \image latex mctdh++_TensorDim_1.eps
 *
 * */

class TensorDim
	// TensorDim (short: TDim) is the dimension-class for a n-th order Tensor
{
public:
	TensorDim()
		: ntensor(0), dimpart(0), dimtot(0), f(0) {}

	TensorDim(const vector<size_t>& dim, size_t ntensor_);

	~TensorDim() = default;

	void Initialize(const vector<size_t>& dim, size_t ntensor);

	void WriteBin(ofstream& os) const;
	void info(ostream& os) const;

	void ReadDim(ifstream& is);

	inline size_t getf() const { return f; }

	inline size_t F() const { return f; }

	inline size_t getdimtot() const { return dimtot; }

	inline size_t getdimpart() const { return dimpart; }

	inline size_t getntensor() const { return ntensor; }

	void setntensor(size_t newntensor);
	void setactive(size_t act, size_t k);

	vector<size_t> getdimlist() const;

	friend bool operator==(const TensorDim& tdima, const TensorDim& tdimb) {
		if (tdima.F() != tdimb.F()) { return false; }
		for (size_t k = 0; k < tdima.F(); k++) {
			if (tdima.Active(k) != tdimb.Active(k)) { return false; }
		}
		if (tdima.getntensor() != tdimb.getntensor()) { return false; }

		return true;
	}

	friend bool operator!=(TensorDim& tdima, TensorDim& tdimb) {
		return !(tdima == tdimb);
	}

	void print(ostream& os = cout)const;

	const TensorABC& getabc(size_t k);
	size_t Active(size_t k) const;
	size_t Behind(size_t k) const;
	size_t Before(size_t k) const;

protected:
	size_t f;
	size_t dimtot;
	size_t dimpart;
	size_t ntensor;
	vector<TensorABC> abc;
};
