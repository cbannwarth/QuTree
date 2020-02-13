//
// Created by Roman Ellerbrock on 2/10/20.
//

#ifndef IOTREE_H
#define IOTREE_H
#include "Tree/MatrixTreeFunctions.h"
#include "Tree/TensorTree.h"

namespace IOTree {

	template <typename T>
	void Occupancy(const TensorTree<T>& Psi, const TTBasis& basis, ostream& os = cout);

	template <typename T>
	void Leafs(const TensorTree<T>& Psi, const MatrixTree<T>& Rho, const TTBasis& basis, ostream& os = cout);

}

#endif //IOTREE_H
