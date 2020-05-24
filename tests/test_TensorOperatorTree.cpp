//
// Created by Roman Ellerbrock on 5/23/20.
//

#include "TreeOperators/TensorOperators/TensorOperatorTree.h"
#include "TreeShape/TreeFactory.h"
#include "TreeOperators/TensorOperators/MatrixListTree.h"
#include "TreeClasses/MatrixTreeFunctions.h"
#include "TreeOperators/TensorOperators/TensorOperatorTreeFunctions.h"
#include <UnitTest++/UnitTest++.h>

SUITE(TensorOperatorTree) {

	double eps = 1e-7;

	class HelperFactory {
	public:
		HelperFactory() {
			Initialize();
		}

		~HelperFactory() = default;
		mt19937 rng_;
		Tree tree_;
		Tree optree_;
		TensorTreecd Psi_;
		TensorOperatorTree H_;

		void Initialize() {
			rng_ = mt19937(1990);
			tree_ = TreeFactory::BalancedTree(12, 2, 2);
			optree_ = TreeFactory::OperatorTree(tree_);
			Psi_ = TensorTreecd(rng_, tree_);

			Matrixcd I = IdentityMatrixcd(2);
			Matrixcd X(2,2);
			X(0, 1) = 1.;
			X(1, 0) = 1.;
			LeafMatrixcd leafI(I);
			LeafMatrixcd leafX(X);

			H_ = TensorOperatorTree(optree_);
			H_.Occupy(optree_);
			for (const Node& node : optree_) {
				if (node.isBottomlayer()) {
					H_.setLeafOperator(leafI, 0, node);
					H_.setLeafOperator(leafX, 1, node);
				}
			}
		}
	};

	TEST(OperatorTree) {
		Tree tree = TreeFactory::BalancedTree(12, 2, 4);
		Tree optree = TreeFactory::OperatorTree(tree);
		for (const Node& node : optree) {
			if (node.isBottomlayer()) {
				CHECK_EQUAL(3, node.shape().order());
			}
		}
	}

	TEST(TensorOperatorTree) {
		Tree tree = TreeFactory::BalancedTree(12, 2, 2);
		Tree optree = TreeFactory::OperatorTree(tree);
		TensorOperatorTree H(optree);
		for (const Node node : optree) {
			if (node.isBottomlayer()) {
				Tensorcd& h = H[node];
				h(0, 0) = 1.;
				h(3, 0) = 1.;

				h(1, 1) = 1.;
				h(2, 1) = 1.;
			}
		}
	}

	TEST(SetLeafOperator) {
		Tree tree = TreeFactory::BalancedTree(12, 2, 2);
		Tree optree = TreeFactory::OperatorTree(tree);

		Matrixcd I = IdentityMatrixcd(2);
		Matrixcd X(2,2);
		LeafMatrixcd leafI(I);
		LeafMatrixcd leafX(X);

		TensorOperatorTree H(optree);
		for (const Node& node : optree) {
			if (node.isBottomlayer()) {
				H.setLeafOperator(leafI, 0, node);
				H.setLeafOperator(leafX, 1, node);
			}
		}
	}

	TEST(MatrixListTree) {
		Tree tree = TreeFactory::BalancedTree(12, 2, 2);
		mt19937 gen(1990);
		TensorTreecd Psi(gen, tree);

		Tree optree = TreeFactory::OperatorTree(tree);

		Matrixcd I = IdentityMatrixcd(2);
		Matrixcd X(2,2);
		X(0, 1) = 1.;
		X(1, 0) = 1.;
		LeafMatrixcd leafI(I);
		LeafMatrixcd leafX(X);

		TensorOperatorTree H(optree);
		for (const Node& node : optree) {
			if (node.isBottomlayer()) {
				H.setLeafOperator(leafI, 0, node);
				H.setLeafOperator(leafX, 1, node);
			}
		}

		MatrixListTree Hrep = TreeFunctions::Represent(Psi, H, tree);

	}

	TEST_FIXTURE(HelperFactory, CNot) {
		auto Hrep = TreeFunctions::Represent(Psi_, H_, tree_);
		for (const Node& node : tree_) {
			Matrixcd I = IdentityMatrixcd(node.shape().lastDimension());
			const MatrixList& hs = Hrep[node];
			CHECK_CLOSE(0., Residual(I, hs[0]), 1e-6);
		}

		MatrixListTree Hmean = TreeFunctions::Contraction(Psi_, H_, Hrep, tree_);
		MatrixTreecd rho = TreeFunctions::Contraction(Psi_, tree_, true);

		for (const Node& node : tree_) {
			if (!node.isToplayer()) {
				const MatrixList& hmeans = Hmean[node];
				size_t dim = hmeans[0].Dim1();
					CHECK_CLOSE(0., Residual(hmeans[0], rho[node]), eps);
					CHECK_CLOSE(0., Residual(hmeans[1], Matrixcd(dim, dim)), eps);
			}
		}
	}


}

