// VRD.cpp
//
// Данный файл является частью библиотеки классов 3D ядра RGK
//(c)Авторское право 2012-2013 АО "Топ Системы". Все права защищены.
//
// Контактная информация:
// rgk@topsystems.ru
// http://www.topsystems.ru
//
/////////////////////////////////////////////////////////////////////////////
// Содержание файла: RGK::Generators::VRD
// Автор: Шаповалов О.В.
// Дата создания: 31.07.2012
// Последнее изменение: Шаповалов О.В.
// Дата последнего изменения: 31.07.2012
/////////////////////////////////////////////////////////////////////////////

#include "../TPLdefines.h"

#include "VRD.h"
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

namespace RGK
{
    namespace Geometry
    {
#define MIN_COUNT 100 /*10000*/
#define MAX_POINTS 1000

#define GAP_COUNT 5
#define EXT_PARAM_MIN 0.01
#define EXT_PARAM_MAX 0.5

        Common::Result VRD::MakeLinkageCurves( Common::Context* iContext, 
            const BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesData& iData, 
            BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport )
        {
            Common::Result result;
            _data = iData;
            _context = iContext;
            //_table = !_data._params.empty();

            _extend = false;

            for (int t=0; t<2; ++t)
            {
                if (!_data._offsetLaws[t])
                {
                    std::vector<Math::Vector2D> points;

                    points.push_back( Math::Vector2D( _data._controlCurveInterval.GetStart(), _data._offsets[t] ) );
                    points.push_back( Math::Vector2D( _data._controlCurveInterval.GetEnd(), _data._offsets[t] ) );

                    Generators::ScalarLaw::Create(iContext, points, _data._offsetLaws[t]);
                }
            }

            if (!_data._uvboxesSet)
            {
                _data._surfaces[0]->GetUVBox(_data._uvboxes[0]);
                _data._surfaces[1]->GetUVBox(_data._uvboxes[1]);
            }

            double mult = EXT_PARAM_MIN;
            
            //где-то здесь надо подменить нурбс поверхности на их расширенные версии
            for (int i=0; i<2; ++i)
            {
                if (_data._surfaces[i]->GetType() == Geometry::NURBSSurfaceType)
                {
                    _sourceSurfaces[i] = _data._surfaces[i]->AsNURBSSurface();

                    NURBSExtensionBuilder::ExtendData dataU(NURBSExtensionBuilder::ExtendData::Linear, NURBSExtensionBuilder::ExtendData::AbsoluteParameter, mult*_data._uvboxes[i].GetU().GetLength());
                    result = NURBSExtensionBuilder::MakeExtendU(_context, dataU, dataU, _sourceSurfaces[i], (NURBSSurfacePtr&)_data._surfaces[i]);
 
                    NURBSExtensionBuilder::ExtendData dataV(NURBSExtensionBuilder::ExtendData::Linear, NURBSExtensionBuilder::ExtendData::AbsoluteParameter, mult*_data._uvboxes[i].GetV().GetLength());
                    result = NURBSExtensionBuilder::MakeExtendV(_context, dataV, dataV, Common::Succeeded(result) ? _data._surfaces[i]->AsNURBSSurface() : _sourceSurfaces[i], (NURBSSurfacePtr&)_data._surfaces[i]);
                }
            }

            _approx = std::make_shared<VRDApprox>(iContext, iData, std::make_shared<VRDObject>(iContext,_data));

            result = _MakeLinkageCurves(oReport);

            return Common::Success;
        }
//
        Common::Result VRD::_MakeLinkageCurves( BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport )
        {
            if (_data._controlCurve == nullptr || _data._surfaces[0] == nullptr || _data._surfaces[1] == nullptr)
                return Instance::CheckResult(Common::NullPointer);
         
            Common::Result result;

            {
                const Interval& lawInterval1 = _data._offsetLaws[0]->GetInterval();
                result = lawInterval1.IsNestedInterval(_data._controlCurveInterval, _context->GetUnitPrecision());
                if (result != Common::Success)
                    return result;

                const Interval& lawInterval2 = _data._offsetLaws[1]->GetInterval();
                result = lawInterval2.IsNestedInterval(_data._controlCurveInterval, _context->GetUnitPrecision());
                if (result != Common::Success)
                    return result;
            }

            _approx->MakeApprox(10, _data._tolerance);
            _approx->FillReport(oReport);

            return Common::Success;
        }

        Common::Result VRD::TripleArray::GetNearest( const Math::Vector3D& approxPoint, Math::Vector3D& centralPoint, Math::Vector3D& tangent, 
            Math::Vector2D& pointFirst, Math::Vector2D& pointSecond ) const
        {
            auto comp = [&approxPoint](const Triple& iTriple1, const Triple& iTriple2)
            {
                return iTriple1._pointCenter.DistanceTo2(approxPoint) < iTriple2._pointCenter.DistanceTo2(approxPoint);
            };

            auto _indexBest = std::min_element(_arr.begin(), _arr.end(), comp) - _arr.begin();
            
            centralPoint = _arr[_indexBest]._pointCenter;

            tangent = _arr[_indexBest]._tangent;

            pointFirst = _arr[_indexBest]._pointsUV[0];
            pointSecond = _arr[_indexBest]._pointsUV[1];

            return Common::Success;
        }
    }
}