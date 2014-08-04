#pragma once

#include "TValueParamObject.h"
#include "Vector3D.h"
#include "Vector2D.h"

class TVec3Vec2Object : public TValueParamObject<ptlVector3D, PTLVector2D> 
{

public:

	using TValueParamObject<ptlVector3D, PTLVector2D>::operator=;
};
