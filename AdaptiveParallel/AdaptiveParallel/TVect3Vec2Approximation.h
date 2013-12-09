#pragma once

#include "TValueParamApproximation.h"
#include "Vector3D.h"
#include "Vector2D.h"
#include "TVec3Vec2Object.h"
#include "PlaneApproximation.h"

class TVec3Vec2Approximation : public TValueParamApproximation<Vector3D, Vector2D>
{
public:

	using TValueParamApproximation<Vector3D, Vector2D>::operator=;

	TVec3Vec2Approximation(std::shared_ptr<TValueParamObject<Vector3D,Vector2D>> iObject) : TValueParamApproximation<Vector3D,Vector2D>(iObject){}

	static std::shared_ptr<TVec3Vec2Approximation> Create(std::shared_ptr<TValueParamObject<Vector3D,Vector2D>> iObject)
	{
		return std::make_shared<TVec3Vec2Approximation>(iObject);
	}

	virtual void MakeApproximation() override
	{
		_approxObject = std::make_shared<PlaneApproximation>(_params,_values,_dimensions,_object);
	}
};