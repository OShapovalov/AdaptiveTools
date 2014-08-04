#pragma once

#include "TValueParamApproximation.h"
#include "Vector3D.h"
#include "Vector2D.h"
#include "TVec3Vec2Object.h"
#include "PlaneApproximation.h"

class TVec3Vec2Approximation : public TValueParamApproximation<ptlVector3D, PTLVector2D>
{
public:

	using TValueParamApproximation<ptlVector3D, PTLVector2D>::operator=;

	//TVec3Vec2Approximation(std::shared_ptr<TValueParamObject<PTLVector3D,PTLVector2D>> iObject) : 
 //   TValueParamApproximation<PTLVector3D,PTLVector2D>(iObject){}

    TVec3Vec2Approximation(std::shared_ptr<TValueParamObject<ptlVector3D,PTLVector2D>> iObject,
        ParallelUtilsPtr iPUtils) : 
    TValueParamApproximation<ptlVector3D,PTLVector2D>(iObject, iPUtils){}

	virtual void MakeApproximation() override
	{
		_approxObject = std::make_shared<PlaneApproximation>(_params,_values,_dimensions,_object);
	}
};