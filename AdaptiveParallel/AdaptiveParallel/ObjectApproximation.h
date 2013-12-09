#pragma once

#include "TValueParamObject.h"

template <class TValue, class TParam>
class ObjectApproximation : public TValueParamObject<TValue,TParam>
{
public:

	ObjectApproximation(const std::vector<TParam>& iParams,
		const std::vector<TValue>& iValues,
		const std::vector<std::size_t>& iDimensions,
		std::shared_ptr<TValueParamObject<TValue,TParam>> iObject) :
	_params(iParams), _values(iValues), _dimensions(iDimensions),_object(iObject)
	{
	}

	virtual void GetIntervals(std::vector<Interval>& oIntervals) const override
	{
		return _object->GetIntervals(oIntervals);
	}

protected:

	ObjectApproximation(){};

	std::vector<TParam> _params;
	std::vector<TValue> _values;

	std::vector<std::size_t> _dimensions;

	std::shared_ptr<TValueParamObject<TValue,TParam>> _object;
};