//
// Created by Roman Ellerbrock on 2020-01-17.
//
#include "UnitTest++/UnitTest++.h"
#include "Matrix.h"

SUITE (Matrix) {
	class MatrixFactory {
	public:
		Matrixcd A;
		Matrixcd B;

		void CreateMatrixA() {
			A = Matrixcd(3, 3);
			for (size_t i = 0; i < A.Dim1(); ++i) {
				for (size_t j = 0; j < A.Dim2(); ++j) {
					A(j, i) = i + j;
				}
			}
		}

		void CreateMatrixB() {
			B = Matrixcd(3, 3);
			for (size_t i = 0; i < B.Dim1(); ++i) {
				for (size_t j = 0; j < B.Dim2(); ++j) {
					B(j, i) = i * j;
				}
			}
		}

		void CreateMatrices() {
			CreateMatrixA();
			CreateMatrixB();
		}

	};

	Matrixcd Create() {
		Matrixcd tmp(3, 3);
		for (size_t i = 0; i < tmp.Dim1(); ++i) {
			for (size_t j = 0; j < tmp.Dim2(); ++j) {
				tmp(j, i) = i + j;
			}
		}
		return tmp;
	}

	constexpr double eps = 1e-7;

	TEST_FIXTURE (MatrixFactory, Matrix_FileIO) {
		/// Test Matrix I/O
		CreateMatrices();
		A.Write("matrix1.dat");
		Matrixcd N("matrix1.dat");
		bool success = A == N;
			CHECK_EQUAL(success, true);
	}

	TEST_FIXTURE (MatrixFactory, Matrix_Add) {
		CreateMatrices();
		auto S = A + B;
		S.Write("matrix_add.dat");
		Matrixcd S_read("matrix_add.dat");
		double r = Residual(S, S);
			CHECK_CLOSE(r, 0., eps);
	}

	TEST_FIXTURE (MatrixFactory, Matrix_Subst) {
		CreateMatrices();
		auto D = A - B;
		D.Write("matrix_subst.dat");
		Matrixcd D_read("matrix_subst.dat");
			CHECK_CLOSE(Residual(D, D_read), 0., eps);
	}

	TEST_FIXTURE (MatrixFactory, Matrix_Prod) {
		CreateMatrices();
		auto D = A * B;
		D.Write("matrix_prod.dat");
		Matrixcd D_read("matrix_prod.dat");
			CHECK_CLOSE(Residual(D, D_read), 0., eps);
	}


	TEST_FIXTURE (MatrixFactory, Matrix_Diagonalization) {
		CreateMatrices();
		auto x = A.cDiag();
		const Matrixcd& Ua = x.first;
		const Vectord& la = x.second;

		Matrixcd U("matrix3a.dat");
		Vectord lambda("matrix3b.dat");
		auto residual_U = Residual(U, x.first);
		auto residual_L = Residual(lambda, x.second);
			CHECK_CLOSE(residual_U, 0., eps);
			CHECK_CLOSE(residual_L, 0., eps);
	}

	TEST_FIXTURE (MatrixFactory, Matrix_RoF) {
		CreateMatrices();
		{
			// Copy asignment operator
			auto Aca = A;
			double r = Residual(A, Aca);
				CHECK_CLOSE(r, 0., eps);
		}

		{
			// Copy constructor
			auto Acc(A);
			double r = Residual(A, Acc);
				CHECK_CLOSE(r, 0., eps);
		}

		{
			// Move asignment operator
			auto Ama = move(Create());
			double r = Residual(A, Ama);
				CHECK_CLOSE(r, 0., eps);
		}

		{
			// Move constructor
			auto Amc(move(Create()));
				CHECK_CLOSE(Residual(A, Amc), 0., eps);
		}
	}



}