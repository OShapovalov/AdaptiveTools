// VRDObject.cpp
//
// Данный файл является частью библиотеки классов 3D ядра RGK
//(c)Авторское право 2012-2013 АО "Топ Системы". Все права защищены.
//
// Контактная информация:
// rgk@topsystems.ru
// http://www.topsystems.ru
//
/////////////////////////////////////////////////////////////////////////////
// Содержание файла: RGK::Generators::VRDObject
// Автор: Шаповалов О.В.
// Дата создания: 31.07.2012
// Последнее изменение: Шаповалов О.В.
// Дата последнего изменения: 31.07.2012
/////////////////////////////////////////////////////////////////////////////

#include "VRDObject.h"
#include "Instance.h"
#include "Geometry/Surfaces/Plane.h"
#include "Geometry/Curves/MakeNURBSCurve.h"
#include "Geometry/Curves/NURBSCurve.h"
#include "Geometry/Curves/ParametricCurve.h"
#include "Geometry/Surfaces/MakeNURBSSurface.h"
#include "Geometry/Surfaces/NURBSSurface.h"
#include "Math/NURBS/NURBSInterpolationBuilder.h"
#include "Math/NURBS/NURBSUtils.h"
#include "Math/BoundingBox.h"
#include "Common/Context.h"
#include "Geometry/Curves/ProjectCurveOnSurfaceUtils.h"
#include "Geometry/Curves/IntersectionCurve.h"
#include "Math/3D/GeometryUtils.h"
#include "Geometry/Curves/Line.h"
#include "Generators/Law.h"
#include "Geometry/Curves/Curve.h"
#include "Model/Session.h"
#include "DebugOutput/ExportToGrapherUtils.h"
#include "DebugOutput/MakeLinkageCurvesTestOperator.h"
#include "Math/BoundingBox/BoundingBoxBuilder.h"
#include "Math/NURBS/NURBSExtensionBuilder.h"
#include "Math/NURBS/BSplineBasisFuncs.h"
#include "Geometry/UVBox.h"
#include "Math/GeometryIntersection/AnalyticIntersections.h"
#include "Math/LinearAlgebra/LinearSolver.h"
#include "Math/NURBS/ArcLengthParameterisationCurve.h"
#include "Math/GeometryIntersection/QuadricIntersectionUtils.h"
#include "Generators/Blending/Geometry/RadiusDiskTools.h"

//#define LINKAGE_CURVE_DEBUG

namespace RGK
{
    namespace Geometry
    {

    }
}