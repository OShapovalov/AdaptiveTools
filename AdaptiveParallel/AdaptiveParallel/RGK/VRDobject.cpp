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
#define MIN_COUNT 100 /*10000*/
#define MAX_POINTS 1000

#define GAP_COUNT 5
#define EXT_PARAM_MIN 0.01
#define EXT_PARAM_MAX 0.5


        Common::Result VRDObject::ProjectPoint( const Math::Vector3D& iPoint, const CurvePtr& iCurve, const Interval& iInterval, const SurfacePtr& iSurface, Math::Vector2D& oPointUV ) const
        {
            Interval curveInterval;

            iCurve->GetInterval(curveInterval);

            double param;
            Math::Vector3D point;

            auto result = iCurve->FindNearestPoint(_context, iPoint, _context->GetLinearPrecision(), param);
            if (result != Common::Success)
                return result;

            Math::GeometryUtils::CheckParameter(param, iInterval, curveInterval);
            Math::GeometryUtils::CheckParameter(param, iInterval, true);

            iCurve->EvaluatePoint(_context, param, point);

            //double dist = point.DistanceTo(iPoint);

            result = iSurface->FindNearestPoint(_context, Surface::FindNearestPointData(point, _context->GetLinearPrecision()), oPointUV);
            if (result != Common::Success)
                return result;

            return Common::Success;
        }


        RGK::Common::Result VRDObject::GetOffsetSide( Common::Context* iContext, const CurvePtr& curve, const Interval& interval, const SurfacePtr& iSurface, 
            double iTolerance, bool iOrientation, const Math::Vector3D& iPlaneNormal, Curve::EquidistantOnSurfaceData::OffsetSide& offsetSide )
        {
            Math::Vector3D point, tang1, direct;
            Math::Vector2D params;

            double param = interval.GetStart();

            curve->EvaluatePoint(iContext, param, point);
            auto result = iSurface->FindNearestPoint(iContext,RGK::Geometry::Surface::FindNearestPointData(point, iTolerance),params);
            if (result != Common::Success)
                return result;

            Math::Vector3D normalSurf1;
            result = iSurface->EvaluateNormal(iContext,params[0],params[1],normalSurf1);
            if (result != Common::Success)
                return result;
            if (normalSurf1.Magnitude() < iContext->GetLinearPrecision())
            {
                param = interval.GetCenter();
                curve->EvaluatePoint(iContext, param, point);
                result = iSurface->FindNearestPoint(iContext,RGK::Geometry::Surface::FindNearestPointData(point, iTolerance),params);
                if (result != Common::Success)
                    return result;
                result = iSurface->EvaluateNormal(iContext,params[0],params[1],normalSurf1);
                if (result != Common::Success)
                    return result;
                if (normalSurf1.Magnitude() < iContext->GetLinearPrecision())
                    return Instance::CheckResult(Common::ZeroVector);
            }
            if (!iOrientation)
                normalSurf1 *= -1;

            result = curve->EvaluateTangent(iContext, param, tang1);
            if (result != Common::Success)
                return result;

            direct = iPlaneNormal.VectorMultiply(tang1);

            offsetSide = ( normalSurf1*direct > 0)  ? Curve::EquidistantOnSurfaceData::Left : Curve::EquidistantOnSurfaceData::Right;

            return Common::Success;
        }

        Common::Result VRDObject::GetEquidistantCurves( const SurfacePtr& iSurface, bool iOrientation, const RGK::Geometry::PlanePtr& plane, double iOffset, 
            const RGK::Geometry::Surface::IntersectSurfaceReport& intersectReport, std::vector<CurvePtr>& eqcurves, std::vector<Interval>& intervals ) const
        {
            for (unsigned i=0; i<intersectReport.GetIntersectCurveCount(); ++i)
            {
                CurvePtr curve = intersectReport.GetIntersectCurve(i);
                const Interval& interval = intersectReport.GetParametersOnCurve(i);            

                Curve::EquidistantOnSurfaceData::OffsetSide offsetSide = Curve::EquidistantOnSurfaceData::Left;
                auto result = GetOffsetSide( _context, curve, interval, iSurface, _data._tolerance, iOrientation, plane->GetLCS().GetZAxis(), offsetSide );
                if (Common::Failed(result))
                    return result;

                Interval eqInterval1;

                Curve::EquidistantOnSurfaceReport eqreport;

                Curve::EquidistantOnSurfaceData eqData(iOffset, interval, plane, Curve::EquidistantOnSurfaceData::ArcLength, _data._tolerance, offsetSide);
                result = curve->MakeEquidistantOnSurface(_context, eqData, eqreport);
                if (result != Common::Success)
                    return result;              

                if (eqreport.GetEquidistantCurvesCount() != 1)
                    return Instance::CheckResult(Common::NotSupported);

                eqcurves.push_back(eqreport.GetEquidistantCurve(0));
                intervals.push_back(eqreport.GetParametersOnCurve(0));
            }

            return Common::Success;
        }

        Common::Result VRDObject::CreatePoints(double iParam, TripleArray& oTriples) const
        {
            oTriples._param = iParam;
            oTriples._count = 0;
            oTriples._arr.clear();

            Math::Vector3D point, tangent;
            _data._controlCurve->EvaluatePoint(_context, iParam, point);

            double offsets[2];

            RGK::Common::Result result = _data._controlCurve->EvaluateTangent(_context, iParam, tangent);
            if (result != Common::Success)
                return result;

            for (int i=0; i<2; ++i)
            {
                result = _data._offsetLaws[i]->Evaluate(_context, iParam, offsets[i]);
                if (result != Common::Success)
                    return result;
            }

            RGK::Geometry::PlanePtr plane;
            result = RGK::Geometry::Plane::CreateByNormalAndPoint(_context, tangent, point, plane);
            if (result != Common::Success)
                return result;

            RGK::Geometry::Surface::IntersectSurfaceReport intersectReports[2];

            for (int i=0; i<2; ++i)
            {
                UVBox uvbox;
                _data._surfaces[i]->GetUVBox(uvbox);

                double mult = _extend ? EXT_PARAM_MAX : EXT_PARAM_MIN;

                if (uvbox.GetU().GetType() == Interval::Infinite)
                {
                    uvbox.GetU() = Interval(_data._uvboxes[i].GetU().GetStart() - mult*_data._uvboxes[i].GetU().GetLength(), _data._uvboxes[i].GetU().GetEnd() + mult*_data._uvboxes[i].GetU().GetLength());  
                }
                
                if (uvbox.GetV().GetType() == Interval::Infinite)
                {
                    uvbox.GetV() = Interval(_data._uvboxes[i].GetV().GetStart() - mult*_data._uvboxes[i].GetV().GetLength(), _data._uvboxes[i].GetV().GetEnd() + mult*_data._uvboxes[i].GetV().GetLength());
                }

                Surface::IntersectSurfaceData data( _data._surfaces[i], /*_data._uvboxes[i]*/uvbox,
                    plane, UVBox(Interval(0,0,Interval::Infinite), Interval(0,0,Interval::Infinite)), _data._tolerance);

                {
                    Math::Vector2D uv1, uv2;
                    Math::Vector3D tangent;
                    bool res;

                    Math::QuadricIntersectionUtils::RefinePoint(_context, point, _context->GetLinearPrecision(), _data._surfaces[i], plane, uv1, uv2, tangent, res);
                    data.SetPoint(point);
                }

                result = Surface::IntersectSurface(_context, data, intersectReports[i]);
                if (Common::Failed(result))
                    return result;    
            }

            
            std::vector<CurvePtr> eqcurves[2];
            std::vector<Interval> intervals[2];

            for (int i=0; i<2; ++i)
            {
                result = GetEquidistantCurves(_data._surfaces[i], _data._orientations[i], plane, offsets[i], intersectReports[i], eqcurves[i], intervals[i] );
                if (Common::Failed(result))
                    return result;
            }


            for (std::size_t i=0; i < eqcurves[0].size(); ++i)
            {
                for (std::size_t k=0; k < eqcurves[1].size(); ++k)
                {
                    Curve::IntersectData intData(eqcurves[0][i], intervals[0][i], eqcurves[1][k], intervals[1][k], _data._tolerance);
                    Curve::IntersectReport intReport;
                    result = Curve::IntersectCurve(_context, intData, intReport);
                    if (result != Common::Success)
                        return result;

                    //ExportToGrapherUtils::OutputCurveToFile(_context, eqcurves[0][i], 100, intervals[0][i], "Curve1.txt");
                    //ExportToGrapherUtils::OutputCurveToFile(_context, eqcurves[1][k], 100, intervals[1][k], "EqCurve1.txt");

                    for (unsigned j = 0; j < intReport.GetIntersectionCount(); ++j)
                    {
                        if (intReport.GetTypeOfIntersection(j) == Curve::IntersectReport::SimpleIntersection)
                        {
                            Triple triple;
                            const Math::Vector3D& point = intReport.GetIntersectionPoint(j);
                            triple._pointCenter = point;
                            
                            result = ProjectPoint(point, intersectReports[0].GetIntersectCurve((unsigned)i), intersectReports[0].GetParametersOnCurve((unsigned)i), _data._surfaces[0], triple._pointsUV[0]);
                            if (Common::Failed(result))
                                return result;

                            result = ProjectPoint(point, intersectReports[1].GetIntersectCurve((unsigned)k), intersectReports[1].GetParametersOnCurve((unsigned)k), _data._surfaces[1], triple._pointsUV[1]);
                            if (Common::Failed(result))
                                return result;

                            oTriples._arr.push_back(triple);
                        }
                    }
                }
            }

            oTriples._count = (int)oTriples._arr.size();

            return Common::Success;
        }
    }
}