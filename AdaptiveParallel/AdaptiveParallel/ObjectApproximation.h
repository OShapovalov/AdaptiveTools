#pragma once

#include "TValueParamObject.h"

template <class TValue, class TParam>
class ObjectApproximation : public TValueParamObject<TValue,TParam>
{
public:

	virtual void GetIntervals(std::vector<TplInterval>& oIntervals) const override
	{
		return _object->GetIntervals(oIntervals);
	}

protected:

	ObjectApproximation(){};

	std::shared_ptr<TValueParamObject<TValue,TParam>> _object;
};