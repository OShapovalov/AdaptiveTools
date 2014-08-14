#pragma once
#include "ParallelTechnology.h"
#include "PTLdefines.h"

#ifdef PTL_TBB

class TBBTechnology : public ParallelTechnology
{
public:

    virtual double Run(IAloneFunction f, int iStart, int iEnd) override;

    virtual void RunSpawn(IVoidFunction f) override; 

    virtual void Synchronize() override;

    virtual std::string GetName() override
    {
        return "TBB";
    }

    virtual enum Technology GetEnum() override
    {
        return ParallelUtils::TBB;
    }
};

#endif