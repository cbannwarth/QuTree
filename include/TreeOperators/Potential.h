//
// Created by Roman Ellerbrock on 3/13/20.
//

#ifndef POTENTIAL_H
#define POTENTIAL_H
#include "Core/Vector.h"
#include "TreeShape/Tree.h"

class Potential {
public:
	Potential() = default;
	virtual ~Potential() = default;

	virtual void Initialize(const Tree& tree) {}

	virtual double Evaluate(const Vectord& Xv, size_t part) const = 0;
};


#endif //POTENTIAL_H
