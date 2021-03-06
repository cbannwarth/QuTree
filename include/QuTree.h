#pragma once
#include <Eigen/Dense>
#include"Core/stdafx.h"
#include"Core/Matrix.h"
#include"Core/Matrix_Implementation.h"
#include"Core/Tensor.h"
#include"Core/TensorShape.h"
#include"Core/Tensor_Extension.h"
#include"Core/Tensor_Extension_Implementation.h"
#include"Core/Tensor_Implementation.h"
#include"Core/Vector.h"
#include"Core/Vector_Implementation.h"
#include"Core/stdafx.h"
#include"Util/BS_integrator.h"
#include"Util/FFT.h"
#include"Util/FFTCooleyTukey.h"
#include"Util/JacobiRotationFramework.h"
#include"Util/Lanzcos.h"
#include"Util/QMConstants.h"
#include"Util/SimultaneousDiagonalization.h"
#include"Util/long_integer.h"

/**
 * \defgroup QuTree
 * \brief QuTree is a C++ Tensor and Tensor Tree library.
 *
 * This library has classes that are similar to libraries like Lapack,
 * Eigen, Armadillo, etc., but its focus is concentrated on handling
 * high-order Tensors and Tensor Trees.
 * */
