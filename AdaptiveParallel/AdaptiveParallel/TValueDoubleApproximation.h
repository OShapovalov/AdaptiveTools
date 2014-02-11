#pragma once

#include "TValueParamApproximation.h"
#include "TValueParamObject.h"
#include <algorithm>
#include "LinearApproximation.h"

template< class TValue >
class TValueDoubleApproximation : public TValueParamApproximation<TValue, double>
{
public:

	using TValueParamApproximation<TValue, double>::operator=;

	TValueDoubleApproximation(std::shared_ptr<TValueParamObject<TValue,double>> iObject) : TValueParamApproximation<TValue,double>(iObject){}

    TValueDoubleApproximation(std::shared_ptr<TValueParamObject<TValue,double>> iObject,
        std::shared_ptr<ParallelUtils> iPUtils) : 
    TValueParamApproximation<TValue,double>(iObject, iPUtils){}

	//static std::shared_ptr<TValueDoubleApproximation<TValue>> Create(std::shared_ptr<TValueParamObject<TValue,double>> iObject)
	//{
	//	return std::make_shared<TValueDoubleApproximation<TValue>>(iObject);
	//}

	virtual void MakeApproximation() override
	{
		_approxObject = std::make_shared<LinearApproximation<TValue>>(_params,_values,_dimensions,_object);
	}

	TplInterval GetInterval(int iDim = 0) const override
	{
		return std::dynamic_pointer_cast< TValueDoubleObject<TValue> >(_object)->GetInterval();
	}

private:

};