#pragma once

//#include "TValueParamObject.h"
#include "ObjectApproximation.h"

template <class TValue>
class LinearApproximation : public ObjectApproximation<TValue,double> //: public TValueParamObject<TValue,double>
{
public:

	using ObjectApproximation<TValue,double>::operator=;

	LinearApproximation(std::vector<double>& iParams,
		std::vector<TValue>& iValues,
		std::vector<std::size_t>& iDimensions,
		std::shared_ptr<TValueParamObject<TValue,double>> iObject) :
	ObjectApproximation<TValue, double>(iParams, iValues, iDimensions, iObject)
	{
	
	}

	virtual TValue Evaluate(const double& iParam) const override
	{
		std::size_t n = std::lower_bound (_params.begin(), _params.end(), iParam) - _params.begin();

		if (n == 0)
			return _values.front();

		double percent = (iParam - _params[n-1]) / (_params[n] - _params[n-1]);

		TValue retValue = _values[n-1]*(1 - percent) + _values[n]*percent;

		return retValue;
	}

private:

};