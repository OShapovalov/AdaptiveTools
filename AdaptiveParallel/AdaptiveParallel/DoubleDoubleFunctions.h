#pragma once

#include "TDoubleDoubleObject.h"

class Tf2 : public TObject
{
public:

	double Evaluate(const double& iParam) const override;

	void GetIntervals(std::vector<Interval>& oIntervals) const override;
};

class Tfsin : public TObject
{
public:

	double Evaluate(const double& iParam) const override;

	void GetIntervals(std::vector<Interval>& oIntervals) const override;
};