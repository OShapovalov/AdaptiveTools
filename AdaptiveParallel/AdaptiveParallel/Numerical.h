#pragma once

template <class TGridPtr>
class Numerical
{
public:
    
    virtual void MakeStep(double t)=0;

protected:
    Numerical(){};
    TGridPtr _grid;
};