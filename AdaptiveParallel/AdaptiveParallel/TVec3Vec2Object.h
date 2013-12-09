#pragma once

#include "TValueParamObject.h"
#include "Vector3D.h"
#include "Vector2D.h"

class TVec3Vec2Object : public TValueParamObject<Vector3D, Vector2D> 
{

public:

	using TValueParamObject<Vector3D, Vector2D>::operator=;
};
