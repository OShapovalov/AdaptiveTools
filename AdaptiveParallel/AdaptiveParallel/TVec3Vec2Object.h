#pragma once

#include "TValueParamObject.h"
#include "Vector3D.h"
#include "Vector2D.h"

class TVec3Vec2Object : public TValueParamObject<tplVector3D, tplVector2D> 
{

public:

	using TValueParamObject<tplVector3D, tplVector2D>::operator=;
};
