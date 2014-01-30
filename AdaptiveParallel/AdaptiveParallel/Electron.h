#pragma once

#include "Gravity.h"
#include "Constants.h"

#include <array>

struct TPL_Point
{
    float x, y, z,
            vx, vy, vz, 
                ax, ay, az;
    //std::array<float,3> _coords;
    //std::array<float,3> _velo;
    //std::array<float,3> _accel;
};

class Electron : public Gravity<TPL_Point>
{
public:
    Electron(const std::vector<TPL_Point>& iPoints, float step);

    Electron(const std::vector<TPL_Point>& iPoints, float step, std::shared_ptr<ParallelUtils> pUtils);

    void pole(TPL_Point& iPoint);

    virtual void FindAcceleration(TPL_Point& iPoint, float step) override;

    virtual void GetAccel(TPL_Point& iPoint1, TPL_Point& iPoint2) override;

    virtual void Transform(TPL_Point& iPoint, float step) override;

private:

    void Init(float step);

    Electron(){};

    float K;
    float QK;//заряд частицы укрупненной
    float MK;//масса частицы укрупненной

    TPL_Point *cuElectrons;
};