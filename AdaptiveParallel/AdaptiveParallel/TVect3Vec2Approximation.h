#pragma once

#include "TValueParamApproximation.h"
#include "Vector3D.h"
#include "Vector2D.h"
#include "TVec3Vec2Object.h"
#include "PlaneApproximation.h"

class TVec3Vec2Approximation : public TValueParamApproximation<tplVector3D, tplVector2D>
{
public:

	using TValueParamApproximation<tplVector3D, tplVector2D>::operator=;

	//TVec3Vec2Approximation(std::shared_ptr<TValueParamObject<tplVector3D,tplVector2D>> iObject) : 
 //   TValueParamApproximation<tplVector3D,tplVector2D>(iObject){}

    TVec3Vec2Approximation(std::shared_ptr<TValueParamObject<tplVector3D,tplVector2D>> iObject,
        ParallelUtilsPtr iPUtils) : 
    TValueParamApproximation<tplVector3D,tplVector2D>(iObject, iPUtils){}

	virtual void MakeApproximation() override
	{
		_approxObject = std::make_shared<PlaneApproximation>(_params,_values,_dimensions,_object);
	}
};