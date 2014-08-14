#pragma once
#include "ParallelTechnology.h"

class SerialTechnology : public ParallelTechnology
{
public:

    virtual double Run(IAloneFunction f, int iStart, int iEnd) override;

    virtual void RunSpawn(IVoidFunction f) override; 

    virtual void Synchronize() override
    {

    }

    virtual std::string GetName() override
    {
        return "Serial";
    }

    virtual enum Technology GetEnum() override
    {
        return Serial;
    }
};