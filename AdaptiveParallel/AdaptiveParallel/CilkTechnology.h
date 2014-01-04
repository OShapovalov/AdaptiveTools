#pragma once

#ifdef CILK

#include "ParallelTechnology.h"

class CilkTechnology : public ParallelTechnology
{
public:

    virtual double Run(std::function<void (int)> f, int iStart, int iEnd) override;

    virtual void RunSpawn(std::function<void (void)> f) override; 

    virtual void Synchronize() override;;

    virtual std::string GetName() override
    {
        return "Cilk Plus";
    }
};

#endif