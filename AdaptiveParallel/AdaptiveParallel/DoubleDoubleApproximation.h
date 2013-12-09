#pragma once

#include "TValueDoubleApproximation.h"

class TDoubleDoubleApproximation : public TValueDoubleApproximation<double>
{
public:
	using TValueDoubleApproximation<double>::operator=;
};