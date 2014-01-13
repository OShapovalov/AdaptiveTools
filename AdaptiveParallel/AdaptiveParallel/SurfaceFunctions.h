#pragma once

#include "Vector3D.h"
#include "Vector2D.h"
#include "TplInterval.h"
#include "TValueParamObject.h"

class TSomeSurface : public TValueParamObject<Vector3D, Vector2D>
{
public:

	Vector3D Evaluate(const Vector2D& params) const override;

	void GetIntervals(std::vector<TplInterval>& oIntervals) const override;

};