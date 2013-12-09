#pragma once

#include "TValueDoubleObject.h"

class TObject: public TValueDoubleObject<double> 
{
public:
	using TValueDoubleObject<double>::operator=;
};