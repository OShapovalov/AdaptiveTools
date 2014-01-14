#pragma once

#include "TValueParamObject.h"

void OutputToGrapher(std::shared_ptr<TValueParamObject<double, double>> iObject, std::string iPath, std::size_t N = 100);

void OutputToGrapher(std::shared_ptr<TValueParamObject<tplVector3D, tplVector2D>> iObject, std::string iPath, std::size_t N = 10000);