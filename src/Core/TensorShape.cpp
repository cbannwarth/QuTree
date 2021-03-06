#include"Core/TensorShape.h"

size_t ContractDimensionsBefore(const vector<size_t>& dim, size_t k) {
	size_t bef = 1;
	for (size_t i = 0; i < k; i++) {
		bef *= dim[i];
	}
	return bef;
}

vector<size_t> ContractDimensionsBefore(const vector<size_t>& dim) {
	vector<size_t> befores(dim.size());
	for (size_t k = 0; k < dim.size(); ++k) {
		befores[k] = ContractDimensionsBefore(dim, k);
	}
	return befores;
}

size_t ContractDimensionsAfter(const vector<size_t>& dim, size_t k) {
	size_t aft = 1;
	for (size_t i = k + 1; i < dim.size(); i++) {
		 aft *= dim[i];
	}
	return aft;
}

vector<size_t> ContractDimensionsAfter(const vector<size_t>& dim) {
	vector<size_t> afters(dim.size());
	for (size_t k = 0; k < dim.size(); ++k) {
		afters[k] = ContractDimensionsAfter(dim, k);
	}
	return afters;
}

TensorShape::TensorShape(const vector<size_t>& dim)
	: TensorShape() {
	Initialize(dim);
}

TensorShape::TensorShape(const initializer_list<size_t>& dims)
	: TensorShape(vector<size_t>(dims)){ }

TensorShape::TensorShape(istream& is)
	: TensorShape() {
	ReadDim(is);
}

TensorShape::TensorShape(const string& file)
	: TensorShape() {
	ifstream is(file);
	ReadDim(is);
}

void TensorShape::Initialize(const vector<size_t>& dim) {
	assert(!dim.empty());
	resize(dim.size());
	for (size_t k = 0; k < dim.size(); ++k) {
		this->operator[](k) = dim[k];
	}
	after_ = ContractDimensionsAfter(dim);
	before_ = ContractDimensionsBefore(dim);
	totalDimension_ = after_.front() * front();
}

void TensorShape::Write(ostream& os) const {
	// Write marker
	os.write("TDIM", 4);

	// Write dof
	int32_t f_write = order();
	os.write((char *) &f_write, sizeof(int32_t));

	// Write active_-dims
	for (size_t k = 0; k < order(); k++) {
		int32_t act = this->operator[](k);
		os.write((char *) &act, sizeof(int32_t));
	}
}

void TensorShape::Write(const string& filename) const {
	ofstream os(filename);
	Write(os);
}

void TensorShape::ReadDim(istream& is) {
	// Check if binary string contains a shape
	char check[5];
	is.read(check, 4);
	string s_check(check, 4);
	string s_key("TDIM");
	assert(s_check == s_key);

	// Read dof
	int32_t f_read;
	is.read((char *) &f_read, sizeof(f_read));

	// Read vector with dimensions of Tensor
	size_t order = f_read;
	vector<size_t> dim_read(order);
	int32_t dim_now;
	for (size_t k = 0; k < order; k++) {
		is.read((char *) &dim_now, sizeof(int32_t));
		dim_read[k]=dim_now;
	}

	// Create this Tensor
	(*this) = TensorShape(dim_read);
}

vector<size_t> TensorShape::dimensions() const {
	vector<size_t> dimlist;
	for (size_t i = 0; i < order(); i++) {
		dimlist.push_back(this->operator[](i));
	}
	return dimlist;
}

size_t TensorShape::before(size_t k) const {
	assert(k < order());
	return before_[k];
}

size_t TensorShape::after(size_t k) const {
	assert(k < order());
	return after_[k];
}

void TensorShape::setDimension(size_t act, size_t k) {
	vector<size_t> dim = dimensions();
	assert(k < dim.size());
	dim[k] = act;
	Initialize(dim);
}

void TensorShape::print(ostream& os) const {
	if (order() > 0) {
		os << "(";
		for (size_t k = 0; k < order() - 1; ++k) {
			os << this->operator[](k) << ", ";
		}
		os << this->operator[](order() - 1);
		os <<  ")" << endl;
	}
}

ostream& operator<<(ostream& os, const TensorShape& tdim) {
	tdim.print(os);
	return os;
}

istream& operator>>(istream& is, TensorShape& tdim) {
	tdim.ReadDim(is);
	return is;
}

bool operator==(const TensorShape& tdima, const TensorShape& tdimb) {
	if (tdima.order() != tdimb.order()) { return false; }
	for (size_t k = 0; k < tdima.order(); k++) {
		if (tdima[k] != tdimb[k]) { return false; }
	}
	return true;
}

bool operator!=(const TensorShape& tdima, const TensorShape& tdimb) {
	return !(tdima == tdimb);
}

TensorShape replaceDimension(TensorShape shape, size_t target, size_t new_dimension) {
	shape[target] = new_dimension;
	auto dims = shape.dimensions();
	shape.Initialize(dims);
	return shape;
}

vector<size_t> indexMapping(size_t I, const TensorShape& shape) {
	/// Perform the super index mapping (backwards).
	/// Break superindex into subindeces I -> (i_1, i_2, ... , i_d)
	vector<size_t> subidx;
	size_t r = I;
	size_t n = 0;
	for (size_t k = 0; k < shape.order(); k++) {
		size_t l = shape[k];
		n = r % l;
		r /= l;
		subidx.push_back(n);
	}
	return subidx;
}

size_t indexMapping(const vector<size_t>& idx, const TensorShape& shape) {
	size_t I = 0;
	for (size_t k = 0; k < shape.order(); ++k) {
		I += shape.before(k) * idx[k];
	}
	return I;
}
