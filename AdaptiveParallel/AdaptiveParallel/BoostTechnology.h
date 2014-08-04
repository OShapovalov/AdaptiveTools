#pragma once

#include "PTLdefines.h"
#include "ParallelTechnology.h"

#ifdef PTL_BOOST

class BoostTechnology : public ParallelTechnology
{
public:

    virtual double Run(std::function<void (int)> f, int iStart, int iEnd) override;

    virtual void RunSpawn(std::function<void (void)> f) override; 

    virtual void Synchronize() override;

    virtual std::string GetName() override
    {
        return "Boost Threads";
    }

    virtual enum Technology GetEnum() override
    {
        return BoostThreads;
    }
};

#endif