#include "Electron.h"
#include "cuElectron.h"

Electron::Electron( const std::vector<TPL_Point>& iPoints, float step ) : Gravity<TPL_Point>(iPoints)
{
    Init(step);
}

Electron::Electron( const std::vector<TPL_Point>& iPoints, float step, std::shared_ptr<ParallelUtils> pUtils ) : 
                        Gravity<TPL_Point>(iPoints, pUtils)
{
    Init(step);
}

void Electron::pole( TPL_Point& iPoint )
{
    float v;

    iPoint.ax = -QK*iPoint.vy*(float)B_const/MK;
    //Y
    if(std::abs(iPoint.z-Z1)<=std::abs(iPoint.z-Z2))
    {
        v = (float)(cos(M_PI_2*std::abs(iPoint.z-Z1)/(Z2-((Z1+Z2)/2)))); 
    }
    else
    {
        v = (float)(cos(M_PI_2*std::abs(iPoint.z-Z2)/(Z2-((Z1+Z2)/2))));
    }
    iPoint.ay = (float)((QK*iPoint.vx*B_const/MK)-(QK*E_const/MK)+(QK*E_const/MK)*std::abs(std::pow(v, 15)));
    //Z
    if(std::abs(iPoint.z-Z1)<=std::abs(iPoint.z-Z2))
    {
        v = (float)(cos(M_PI_2*std::abs(iPoint.z-Z1)/(Z2-((Z1+Z2)/2))));
        iPoint.az = std::abs((QK*E_const/MK)*std::pow(v, 15));
    }
    else
    {
        v = (float)(cos(M_PI_2*std::abs(iPoint.z-Z2)/(Z2-((Z1+Z2)/2))));
        iPoint.az = -std::abs((QK*E_const/MK)*std::pow(v, 15));
    }
}

void Electron::FindAcceleration( TPL_Point& iPoint, float step )
{
    pole(iPoint);
}

void Electron::GetAccel( TPL_Point& iPoint1, TPL_Point& iPoint2 )
{
    std::array<float, 3> delta = { 
        iPoint1.x - iPoint2.x,
        iPoint1.y - iPoint2.y,
        iPoint1.z - iPoint2.z 
    };
    float r2 = std::pow( delta[0], 2 ) +
        std::pow( delta[1], 2 ) +
        std::pow( delta[2], 2 );

    float r = sqrt(r2);

    float F = (float)(num_k*Q*Q / r2);

    iPoint1.ax += F * delta[0] / r;
    iPoint2.ax +=  - F * delta[0] / r;

    iPoint1.ay += F * delta[1] / r;
    iPoint2.ay +=  - F * delta[1] / r;

    iPoint1.az += F * delta[2] / r;
    iPoint2.az +=  - F * delta[2] / r;
}

void Electron::Transform( TPL_Point& iPoint, float step )
{
    iPoint.x += iPoint.vx * step + iPoint.ax * 0.5f * step * step;
    iPoint.vx += iPoint.ax * step;
    iPoint.ax = 0.0;

    iPoint.y += iPoint.vy * step + iPoint.ay * 0.5f * step * step;
    iPoint.vy += iPoint.ay * step;
    iPoint.ay = 0.0;

    iPoint.z += iPoint.vz * step + iPoint.az * 0.5f * step * step;
    iPoint.vz += iPoint.az * step;
    iPoint.az = 0.0;
}

void Electron::Init(float step)
{
    K = (float)(std::abs(I*step/(Q*_grid.size())));
    QK = (float)(Q*K);//заряд частицы укрупненной
    MK = (float)(M*K);//масса частицы укрупненной

    /*cuElectron::*/
    cuMalloc( (void**)cuElectrons, (int)_grid.size()*(int)sizeof(TPL_Point) );
}
