#pragma once

#include "TValueDoubleObject.h"

class TDoubleDoubleObject: public TValueDoubleObject<double> 
{
public:
	using TValueDoubleObject<double>::operator=;
};