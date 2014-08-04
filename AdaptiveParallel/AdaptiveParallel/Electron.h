#pragma once

#include "Gravity.h"
#include "Constants.h"

#include <array>

struct PTL_Point
{
    float x, y, z,
            vx, vy, vz, 
                ax, ay, az;
    //std::array<float,3> _coords;
    //std::array<float,3> _velo;
    //std::array<float,3> _accel;
};

class Electron : public Gravity<PTL_Point>
{
public:
    Electron(const std::vector<PTL_Point>& iPoints, float step);

    Electron(const std::vector<PTL_Point>& iPoints, float step, ParallelUtilsPtr pUtils);

    void pole(PTL_Point& iPoint);

    virtual void FindAcceleration(PTL_Point& iPoint, float step) override;

    virtual void GetAccel(PTL_Point& iPoint1, PTL_Point& iPoint2) override;

    virtual void Transform(PTL_Point& iPoint, float step) override;

private:

    void Init(float step);

    //Electron(){};

    float K;
    float QK;//заряд частицы укрупненной
    float MK;//масса частицы укрупненной

    PTL_Point *cuElectrons;
};