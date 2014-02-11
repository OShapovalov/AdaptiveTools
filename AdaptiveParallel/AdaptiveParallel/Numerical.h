#pragma once
#include "ParallelUtils.h"

template <class TGridPtr>
class Numerical
{
public:
    
    virtual void MakeStep(float t)=0;

protected:
    Numerical()
    {
        _pUtils = std::make_shared<ParallelUtils>("Numerical.ini");
    };

    Numerical(std::shared_ptr<ParallelUtils> pUtils) : _pUtils(pUtils)
    {
    };

    TGridPtr _grid;

    std::shared_ptr<ParallelUtils> _pUtils; 
};