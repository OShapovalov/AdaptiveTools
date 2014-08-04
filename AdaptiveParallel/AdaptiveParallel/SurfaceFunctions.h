#pragma once

#include "Vector3D.h"
#include "Vector2D.h"
#include "PTLInterval.h"
#include "TValueParamObject.h"

class TSomeSurface : public TValueParamObject<ptlVector3D, PTLVector2D>
{
public:

	ptlVector3D Evaluate(const PTLVector2D& params) const override;

	void GetIntervals(std::vector<PTLInterval>& oIntervals) const override;

};