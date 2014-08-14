#pragma once
#include <functional>
#include "ParallelUtils.h"
#include "PTLForward.h"

//#define BOOST
//#define CILK

class ParallelTechnology
{
public:

    virtual double Run(IAloneFunction f, int iStart, int iEnd) =0;

    virtual void RunSpawn(IVoidFunction f) =0;

    virtual void Synchronize() =0;

    virtual std::string GetName() =0;

    virtual enum Technology GetEnum() =0;
};