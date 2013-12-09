#pragma once

#include "TValueParamObject.h"

template< class TValue >
class TValueDoubleObject : public TValueParamObject<TValue, double> 
{

public:

	using TValueParamObject<TValue, double>::operator=;
};
