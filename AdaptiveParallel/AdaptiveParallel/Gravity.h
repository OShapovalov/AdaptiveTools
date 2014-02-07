#pragma once

#include <vector>
#include "NonGrid.h"

//typedef std::vector<TPoint> TPointArray;

template <class TPoint>
class Gravity : public TNonGrid< std::vector<TPoint> >
{
public:
    Gravity(const std::vector<TPoint>& iPoints)
    {
        _grid = iPoints;
    };

    Gravity(const std::vector<TPoint>& iPoints, std::shared_ptr<ParallelUtils> pUtils) : TNonGrid<std::vector<TPoint>>(pUtils)
    {
        _grid = iPoints;
    };

    virtual void GetAccel(TPoint& iPoint1, TPoint& iPoint2) = 0;

    virtual void Transform(TPoint& iPoint, float step) = 0;

    virtual void FindAcceleration(TPoint& iPoint, float step) =0;

    virtual void MakeStep(float t) override
    {
        _pUtils->RunInParallel(
            [&](int i)
            {
                FindAcceleration(_grid[i], t);
            }, 
                0, (int)_grid.size() );

        _pUtils->RunInParallel(
            [&](int i)
        {
            for (std::size_t k=0; k<i; ++k)
            {
                GetAccel(_grid[i], _grid[k]);
            }
        }, 
            0, (int)_grid.size() );

        _pUtils->RunInParallel(
            [&](int i)
        {
            Transform(_grid[i], t);
        }, 
            0, (int)_grid.size() );
    }
protected:
    Gravity(){};
    //TGridPtr _grid;
};