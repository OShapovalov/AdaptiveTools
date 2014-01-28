#pragma once
#include "Gravity.h"

struct TPL_Point
{
    std::array<double,3> _coords;
    std::array<double,3> _velo;
    std::array<double,3> _accel;
};

class Electron : public Gravity<TPL_Point>
{
public:
    Electron(const std::vector<TPL_Point>& iPoints):Gravity<TPL_Point>(iPoints){};

    virtual void GetAccel(TPL_Point& iPoint1, TPL_Point& iPoint2) override
    {
        for (int i=0;i<3;++i)
        {
            iPoint1._accel[i] = 7777;vfhfshsfh
        }         
    }

    virtual void Transform(TPL_Point& iPoint, double step) override
    {
        for (int i=0;i<3;++i)
        {
            iPoint._coords[i] += iPoint._velo[i] * step + iPoint._accel[i] * 0.5 * step * step;
            iPoint._velo[i] += iPoint._accel[i] * step;
            iPoint._accel[i] = 0.0;
        }    
    }

private:
    Electron(){};
};