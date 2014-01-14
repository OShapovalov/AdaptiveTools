#pragma once

#include "Vector3D.h"
#include "Vector2D.h"
#include "TplInterval.h"
#include "TValueParamObject.h"

class TSomeSurface : public TValueParamObject<tplVector3D, tplVector2D>
{
public:

	tplVector3D Evaluate(const tplVector2D& params) const override;

	void GetIntervals(std::vector<TplInterval>& oIntervals) const override;

};