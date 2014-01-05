#pragma once
#include <functional>

#define BOOST

class ParallelTechnology
{
public:

    virtual double Run(std::function<void (int)> f, int iStart, int iEnd) =0;

    virtual void RunSpawn(std::function<void (void)> f) =0; 

    virtual void Synchronize() =0;

    virtual std::string GetName() =0;
};