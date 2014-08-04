#pragma once

#include "TValueParamObject.h"

void OutputToGrapher(std::shared_ptr<TValueParamObject<double, double>> iObject, std::string iPath, std::size_t N = 100);

void OutputToGrapher(std::shared_ptr<TValueParamObject<ptlVector3D, PTLVector2D>> iObject, std::string iPath, std::size_t N = 10000);