#pragma once
#include "Numerical.h"

//typedef std::vector<TPoint> TPointArray;

template <class TPoint>
class Gravity : public Numerical< std::vector<TPoint> >
{
public:
    Gravity(const std::vector<TPoint>& iPoints)
    {
        _grid = iPoints;
    };

    virtual void GetAccel(TPoint& iPoint1, TPoint& iPoint2) = 0;
    virtual void Transform(TPoint& iPoint, double step) = 0;

    virtual void MakeStep(double t) override
    {
        for (std::size_t i=0; i<_grid.size(); ++i)
        {
            for (std::size_t k=0; k<i; ++k)
            {
                GetAccel(_grid[i], _grid[k]);
            }
        }

        for (std::size_t i=0;i<_grid.size();++i)
        {
            Transform(_grid[i], t);
        }
    }
protected:
    Gravity(){};
    //TGridPtr _grid;
};