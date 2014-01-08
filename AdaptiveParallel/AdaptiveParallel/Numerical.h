#pragma once
#include <vector>

template <class TGridPtr>
class Numerical
{
public:
    Numerical(){};
    void GetStep(double t)=0;
private:
    TGridPtr _grid;
};