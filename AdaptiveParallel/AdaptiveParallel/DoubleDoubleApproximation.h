#pragma once

#include "TValueDoubleApproximation.h"

class TDoubleDoubleApproximation : public TValueDoubleApproximation<double>
{
public:
    using TValueDoubleApproximation<double>::operator=;

    TDoubleDoubleApproximation(std::shared_ptr<TValueParamObject<double,double>> iObject) : 
    TValueDoubleApproximation<double>(iObject){}
};