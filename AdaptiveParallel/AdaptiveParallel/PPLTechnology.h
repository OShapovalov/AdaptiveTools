#pragma once
#include "ParallelTechnology.h"
#include "PTLdefines.h"

#ifdef PTL_PPL

class PPLTechnology : public ParallelTechnology
{
public:

    virtual double Run(IAloneFunction f, int iStart, int iEnd) override;

    virtual void RunSpawn(IVoidFunction f) override; 

    virtual void Synchronize() override;

    virtual std::string GetName() override
    {
        return "PPL";
    }

    virtual enum Technology GetEnum() override
    {
        return PPL;
    }
};

#endif