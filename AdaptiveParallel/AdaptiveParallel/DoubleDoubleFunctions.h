#pragma once

#include "TDoubleDoubleObject.h"

class Tf2 : public TDoubleDoubleObject
{
public:

	double Evaluate(const double& iParam) const override;

	void GetIntervals(std::vector<PTLInterval>& oIntervals) const override;
};

class Tfsin : public TDoubleDoubleObject
{
public:

	double Evaluate(const double& iParam) const override;

	void GetIntervals(std::vector<PTLInterval>& oIntervals) const override;
};