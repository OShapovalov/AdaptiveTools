#pragma once

//#ifdef BOOST

#include "ParallelTechnology.h"

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
};

//#endif