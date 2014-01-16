﻿// VRD.cpp
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
//
//        Common::Result VRD::GetToleranceForSurface(Common::Context *iContext, SurfacePtr iSurface, const std::vector<Math::Vector2D>& iUVPoints, double iTolerance, double& oParamTolerance)
//        {
//            Common::Result result;
//         
//            if (iSurface->GetType() == RGK::Geometry::Geometry::NURBSSurfaceType)
//
//            {
//                NURBSSurfacePtr nSurf1 = iSurface->AsNURBSSurface();
//                result = NURBSUtils::ConvertTolerance(iContext,nSurf1,iTolerance,oParamTolerance);
//                if (result != Common::Success)
//                    return result;
//
//                oParamTolerance /= 2.0;
//            }
//            else
//            {
//                double maxU = 0, maxV = 0;
//                Math::Vector3D derU, derV;
//                for (std::size_t i=0; i < iUVPoints.size(); ++i )
//                {
//                    result = iSurface->Evaluate(iContext,iUVPoints[i][0],iUVPoints[i][1],1,0,derU);
//                    if (result != Common::Success)
//                        return result;
//                    result = iSurface->Evaluate(iContext,iUVPoints[i][0],iUVPoints[i][1],0,1,derV);
//                    if (result != Common::Success)
//                        return result;
//
//                    maxU = std::max(maxU,derU.Magnitude());
//                    maxV = std::max(maxV,derV.Magnitude());
//                }
//
//                oParamTolerance = iTolerance / (maxU + maxV) / 10.0;
//            }
//            return Common::Success;
//        }
//
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

            _approx->MakeApprox(_data._tolerance);

            return Common::Success;
        }
////
//        Common::Result VRD::UpdateMaxDistance(Common::Context* iContext, const CurvePtr& iCurve, double iParam, const Math::Vector3D& iPoint, double& ioDist)
//        {
//            Common::Result result;
//            Math::Vector3D locPoint;
//
//            result = iCurve->EvaluatePoint(iContext,iParam,locPoint);
//            if (result != Common::Success)
//                return result;
//            double dist1 = (iPoint - locPoint).Magnitude();
//            ioDist = std::max(ioDist,dist1);
//
//            return Common::Success;
//        }
//
//        Common::Result VRD::GetMaxDistance( AllPointsPtr iAllPoints, double iParam, 
//            const Math::Vector3D& pointCenter, const Math::Vector2D& pointFirst, const Math::Vector2D& pointSecond, double& oDist )
//        {
//            Common::Result result;
//
//            Math::Vector3D point3d1, point3d2;
//
//            result = _data._surfaces[0]->EvaluatePoint(_context, pointFirst[0], pointFirst[1], point3d1);
//            if (result != Common::Success)
//                return result;
//
//            result = _data._surfaces[1]->EvaluatePoint(_context, pointSecond[0], pointSecond[1], point3d2);
//            if (result != Common::Success)
//                return result;
//
//            oDist = 0;
//
//            result = UpdateMaxDistance(_context, iAllPoints->_centralCurve, iParam, pointCenter, oDist);
//            if (result != Common::Success)
//                return result;
//
//            result = UpdateMaxDistance(_context, iAllPoints->_pcurves[0], iParam, point3d1, oDist);
//            if (result != Common::Success)
//                return result;
//
//            result = UpdateMaxDistance(_context, iAllPoints->_pcurves[1], iParam, point3d2, oDist);
//            if (result != Common::Success)
//                return result;
//
//            return Common::Success;
//        }
//
//
//        Common::Result VRD::MakeAll(BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport )
//        {
//            int N = MIN_COUNT;
//
//            Math::Vector3D tangent,point;
//            std::vector<double> params(N);
//
//            Common::Result result = GetInitialParams(N, params);
//            if (Common::Failed(result))
//                return result;
//
//            //std::vector<int> counts(N); // пока лучше оставить для дебага
//
//            std::vector<TripleArray> allTriples(params.size());
//
//#ifndef _DEBUG
//            auto threads(std::min(int(_context->GetMaxThreads()),N));
//#pragma omp parallel for if (threads > 1) num_threads(threads) schedule(dynamic)
//#endif
//            for (int i=0; i<N; i++)
//            {
//                CreatePoints(params[i], allTriples[i]);
//                //counts[i] = allTriples[i]._count;
//            }
//
//            {
//                bool find = false;
//
//                for (int i=0; i<N; i++)
//                {
//                    if (allTriples[i]._count > 0)
//                    {
//                        find = true;
//                        break;
//                    }
//                }
//
//                if (!find /*|| true*/ /*&& false*/)
//                {
//                    _extend = true;
//
//                    double mult = _extend ? EXT_PARAM_MAX : EXT_PARAM_MIN;
//
//                    //где-то здесь надо подменить нурбс поверхности на их расширенные версии
//                    for (int i=0; i<2; ++i)
//                    {
//                        if (_data._surfaces[i]->GetType() == Geometry::NURBSSurfaceType)
//                        {
//                            NURBSExtensionBuilder::ExtendData dataU(NURBSExtensionBuilder::ExtendData::Linear, NURBSExtensionBuilder::ExtendData::AbsoluteParameter, mult*_data._uvboxes[i].GetU().GetLength());
//                            result = NURBSExtensionBuilder::MakeExtendU(_context, dataU, dataU, _sourceSurfaces[i], (NURBSSurfacePtr&)_data._surfaces[i]);
//
//                            NURBSExtensionBuilder::ExtendData dataV(NURBSExtensionBuilder::ExtendData::Linear, NURBSExtensionBuilder::ExtendData::AbsoluteParameter, mult*_data._uvboxes[i].GetV().GetLength());
//                            result = NURBSExtensionBuilder::MakeExtendV(_context, dataV, dataV, Common::Succeeded(result) ? _data._surfaces[i]->AsNURBSSurface() : _sourceSurfaces[i], (NURBSSurfacePtr&)_data._surfaces[i]);
//
//                            //ExportToGrapherUtils::OutputSurfaceToFile(_context, "Source.txt", _sourceSurfaces[i]);
//                            //ExportToGrapherUtils::OutputSurfaceToFile(_context, "Extended.txt", _data._surfaces[i]);
//                        }
//                    }
//
//                    const int M = 14;
//
//                    TripleArray trArr;
//
//                    Interval curveInterval;
//                    _data._controlCurve->GetInterval(curveInterval);
//
//                    std::vector<double> new_params;
//
//                    double param;
//
//                    for (int i=0; i<2; ++i)
//                    {
//                        RGK::Geometry::UVBox box = _data._uvboxes[i];
//
//                        double startU = box.GetU().GetStart(), endU = box.GetU().GetEnd();
//                        double startV = box.GetV().GetStart(), endV = box.GetV().GetEnd();
//
//                        double stepU = (endU - startU) / M;
//                        double stepV = (endV - startV) / M;
//
//                        for(double u = startU; u <= endU; u += stepU)
//                            for(double v = startV; v <= endV; v += stepV)
//                            {
//                                RGK::Math::Vector3D point;
//                                auto result = _data._surfaces[i]->EvaluatePoint(_context, u, v, point);
//                                if(result == RGK::Common::Success)
//                                {
//                                    _data._controlCurve->FindNearestPoint(_context, point, _context->GetLinearPrecision(), param );
//                                    new_params.push_back(param);
//
//                                    //Math::GeometryUtils::CheckParameter(param, _data._controlCurveInterval, curveInterval);
//                                    //Math::GeometryUtils::CheckParameter(param, _data._controlCurveInterval, true);
//
//                                    //auto pos = std::lower_bound(params.begin(), params.end(), param) - params.begin();
//
//                                    //CreatePoints(param, trArr);
//                                    //if (trArr._count > 0 && allTriples[pos]._count == 0 && allTriples[pos+1]._count == 0)
//                                    //{
//                                    //    params.insert(params.begin()+pos, param);
//                                    //    allTriples.insert(allTriples.begin()+pos, trArr);
//                                    //    ++N;
//                                    //    break;
//                                    //}
//                                }
//                            }
//                    }
//
//                    std::sort(new_params.begin(), new_params.end());
//                    std::swap(params, new_params);
//                    N = (int)params.size();
//                    allTriples.resize(N);
//
//#ifndef _DEBUG
//                    auto threads(std::min(int(_context->GetMaxThreads()),N));
//#pragma omp parallel for if (threads > 1) num_threads(threads) schedule(dynamic)
//#endif
//                    for (int i=0; i<N; i++)
//                    {
//                        CreatePoints(params[i], allTriples[i]);
//                        //counts[i] = allTriples[i]._count;
//                    }
//                }
//            }
//
//            for (std::size_t i=0; i<allTriples.size(); ++i)
//            {
//                result = AddPoints(i>0 ? params[i-1] : _data._controlCurveInterval[0], allTriples[i], oReport);
//                if (Common::Failed(result))
//                    return result;
//            }
//
//            result = FinishAll(oReport);
//            if (Common::Failed(result))
//                return result;
//
//            return Common::Success;
//        }

        //Common::Result VRD::GetNearestPoint( const TripleArray& triple, const Math::Vector3D& approxPoint, Math::Vector3D& centralPoint, Math::Vector3D& tangent,
        //    Math::Vector2D& pointFirst, Math::Vector2D& pointSecond )
        //{
        //    return triple.GetNearest(approxPoint, centralPoint, tangent, pointFirst, pointSecond);
        //}

        //Common::Result VRD::CreateCurve( AllPointsPtr iAllPoints, BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport )
        //{
        //    //ExportToGrapherUtils::ExportPointsToGrapher("points.txt", iAllPoints->_centralPoints);

        //    iAllPoints->_created = true;

        //    std::vector<double>& params = iAllPoints->_params;
        //    std::vector<Math::Vector2D> *pointsUV = iAllPoints->_pointsUV;

        //    //if (params.size() > 1)
        //    //{
        //    //    Math::Vector3D tangent;
        //    //    _data._controlCurve->EvaluateTangent(_context, params.front(), tangent);
        //    //    if (tangent * (iAllPoints->_centralPoints[1]-iAllPoints->_centralPoints[0]) < 0.0)
        //    //        return Common::Success;
        //    //}

        //    Common::Result result;

        //    {
        //        result = RefineEnds(iAllPoints, true);
        //        if( result != Common::Success )
        //            return result;
        //        result = RefineEnds(iAllPoints, false);
        //        if( result != Common::Success )
        //            return result;
        //    }                        
        //    
        //    if (params.size() < 2)
        //        return Common::Success;
        //    
        //    for (int i=0; i<2; ++i)
        //    {            
        //        result = ProjectCurveOnSurfaceUtils::CheckUVPoints(_context, _data._surfaces[i], pointsUV[i], params, false);
        //        if( result != Common::Success )
        //            return result;
        //    }

        //    double paramTolerances[2];
        //    for (int i=0; i<2; ++i)
        //    {
        //        result = GetToleranceForSurface(_context, _data._surfaces[i], pointsUV[i], _data._tolerance, paramTolerances[i]);
        //        if( result != Common::Success )
        //            return result;
        //    }

        //    double unitTolerance = _data._controlCurveInterval.GetLinearPrecision(_context);
        //    double uTolerance = std::max(4.0 * unitTolerance, 2 * _context->GetLinearPrecision());

        //    do
        //    {
        //        result = InterpolateCurves(iAllPoints, paramTolerances);
        //        if (Common::Failed(result))
        //            return result;
        //        
        //        //if (false)
        //        //break;

        //        std::vector<double> splitParams;
        //        std::vector<double> addParams;
        //        
        //        std::vector<double> checkParams;
        //        for (int i=0;i<2;++i)
        //        {
        //            result = FindGap(params, pointsUV[i], iAllPoints->_pcurves[i], checkParams);
        //            if (Common::Failed(result))
        //                return result;
        //        }

        //        for (std::size_t i=0; i< checkParams.size(); ++i)
        //        {
        //            const TripleArray& tripleArr = GetTripleArray(checkParams[i]);

        //            std::size_t index = std::lower_bound(params.begin(), params.end(), checkParams[i]) - params.begin();

        //            if (tripleArr._count < iAllPoints->_counts[index-1] && tripleArr._count < iAllPoints->_counts[index])
        //            {
        //                if (Check(iAllPoints, tripleArr))
        //                    splitParams.push_back(checkParams[i]);
        //            }
        //        }

        //        if (splitParams.empty())
        //        {
        //            //CacheMiddleTriples(params, uTolerance);

        //            result = GetSplitAddParams(iAllPoints, uTolerance, splitParams, addParams);
        //            if (Common::Failed(result))
        //                return result;
        //        }

        //        if (splitParams.empty() && addParams.empty())
        //            break;

        //        CurveTriple cTr;

        //        for (std::size_t i = 0; i<addParams.size();++i)
        //        {
        //            //result = GetTriple(addParams[i], iAllPoints, cTr);
        //            //if (Common::Succeeded(result))
        //            //{
        //            //    std::size_t index = std::lower_bound(iAllPoints->_params.begin(), iAllPoints->_params.end(), addParams[i]) - iAllPoints->_params.begin();

        //            //    iAllPoints->InsertTriple(index, cTr._triple, cTr._param, iAllPoints->_counts[index], _data);
        //            //}
        //            //else
        //                InsertAllTriples(GetTripleArray(addParams[i]), iAllPoints);
        //        }

        //        if (!splitParams.empty())
        //        {
        //            auto it = std::find(_allCurves.begin(), _allCurves.end(), iAllPoints);
        //            if (it !=  _allCurves.end())
        //                _allCurves.erase(it);

        //            std::vector<AllPointsPtr> newCurves;
        //            result = SplitCurves(iAllPoints, splitParams, newCurves);
        //            if (Common::Failed(result))
        //                return result;

        //            for (std::size_t i=0; i<newCurves.size(); ++i)
        //            {
        //                result = CreateCurve(newCurves[i], oReport );
        //                if (Common::Failed(result))
        //                    return result;
        //            }

        //            return Common::Success;
        //        }
        //    }
        //    while ( params.size() < MAX_POINTS );

        //    oReport._linkageCurves[0].push_back(iAllPoints->_pcurves[0]);
        //    oReport._linkageCurves[1].push_back(iAllPoints->_pcurves[1]);
        //    oReport._spineCurves.push_back(iAllPoints->_centralCurve);

        //    return Common::Success;
        //}

        //Common::Result VRD::CacheMiddleTriples(const std::vector<double>& iParams, double uTolerance)
        //{
        //    std::vector<double> params;
        //    for (std::size_t i=1; i<iParams.size(); ++i)
        //    {
        //        if (iParams[i]-iParams[i-1] <= uTolerance )
        //            continue;

        //        double param = ( iParams[i-1] + iParams[i] ) * 0.5;

        //        auto comp = [param](const TripleArray& tripleArr)
        //        {
        //            return (param == tripleArr._param);
        //        };

        //        auto it = std::find_if(_cachePoints.begin(), _cachePoints.end(), comp);

        //        if (it == _cachePoints.end())
        //            params.push_back(param);
        //    }

        //    return CacheTriples(params);
        //}
//
//        Common::Result VRD::CacheTriples(const std::vector<double>& iParams)
//        {
//            std::vector<Common::Result> results(iParams.size());
//            std::vector<TripleArray> newTriples(iParams.size());
//
//#ifndef _DEBUG
//            auto threads(_context->GetMaxThreads());
//#pragma omp parallel for num_threads(threads)
//#endif
//            for (int i=0;i<(int)iParams.size();++i)
//            {
//                results[i] = CreatePoints(iParams[i], newTriples[i]);
//            }
//
//            for (std::size_t i=0;i<iParams.size();++i)
//            {
//                if (Common::Failed(results[i]))
//                    return results[i];
//            }
//
//            _cachePoints.insert(_cachePoints.end(), newTriples.begin(), newTriples.end());
//
//            return Common::Success;
//        }

        //const VRD::TripleArray& VRD::GetTripleArray(double param)
        //{
        //    auto comp = [param](const TripleArray& tripleArr)
        //    {
        //        return (param == tripleArr._param);
        //    };

        //    auto it = std::find_if(_cachePoints.begin(), _cachePoints.end(), comp);

        //    if (it == _cachePoints.end())
        //    {
        //        TripleArray trArr;

        //        CreatePoints(param, trArr);

        //        _cachePoints.push_back(trArr);

        //        return _cachePoints.back();
        //    }
        //    else
        //    {
        //        return *it;
        //    }
        //}

        //Common::Result VRD::AddTriple(double param, AllPointsPtr iAllpoints, const Math::Vector2D& iUV1, const Math::Vector2D& iUV2, VRD::CurveTriple& oCurveTriple)
        //{   //return Common::InternalError;                        
        //    RadiusDiskTools diskTools(_data._surfaces[0], _data._surfaces[1], _data._controlCurve, _data._controlCurveInterval, _data._offsetLaws[0], _data._offsetLaws[1], 
        //        _data._orientations[0], _data._orientations[1], _data._tolerance);

        //    //diskTools.SetUVBoxes(_data._uvboxes[0], _data._uvboxes[1]);

        //    CurveTriple cTr;
        //    cTr._param = param;
        //    cTr._allPoints = iAllpoints;

        //    Math::Vector2D inUV[2] = {iUV1, iUV2};
        //    auto result = diskTools.IterateToCircleTangentPoint(_context, param, inUV, cTr._triple._pointCenter, cTr._triple._pointsUV );
        //    if (Common::Failed(result))
        //        return result;

        //    _cacheTriples.push_back(cTr);

        //    oCurveTriple = _cacheTriples.back();

        //    return Common::Success;
        //}

        //Common::Result VRD::GetTriple(double param, AllPointsPtr iAllpoints, VRD::CurveTriple& oCurveTriple) const
        //{                           
        //    auto comp = [param, iAllpoints](const CurveTriple& tripleArr)
        //    {
        //        return (iAllpoints == tripleArr._allPoints) && (param == tripleArr._param);
        //    };

        //    auto it = std::find_if(_cacheTriples.begin(), _cacheTriples.end(), comp);

        //    if (it == _cacheTriples.end())
        //        return Common::NotConverged;

        //    oCurveTriple = *it;

        //    return Common::Success;
        //}

        //Common::Result VRD::GetInitialParams(int& N, std::vector<double> &params )
        //{
        //    ArcLengthParameterisationCurvePtr arc;
        //    Common::Result result = ArcLengthParameterisationCurve::Create(_context, _data._controlCurve, _data._controlCurveInterval, _data._tolerance, arc);
        //    if (Common::Failed(result))
        //        return result;

        //    double length = arc->GetLength();

        //    for(int i=0; i<N-1; ++i)
        //    {
        //        result = arc->GetParameterByLength((double)i/(N-1)*length, params[i]);
        //        if (Common::Failed(result))
        //            return result;
        //    }            

        //    params[N-1] = _data._controlCurveInterval.GetEnd();

        //    return Common::Success;
        //}

        //Common::Result VRD::RefineEnds(AllPointsPtr iAllPoints, bool iStart )
        //{
        //    double paramTolerance = _data._controlCurveInterval.GetLinearPrecision(_context);

        //    double endParam = iStart ? iAllPoints->_params[0] : iAllPoints->_params.back();
        //    double boundParam = iAllPoints->_bounds[ iStart ? 0 : 1 ];
        //    double mult = iStart ? 1.0 : -1.0;

        //    std::size_t index = iStart ? 0 : iAllPoints->_params.size() -1;

        //    if ( Common::Failed( _data._uvboxes[0].ContainParam(iAllPoints->_pointsUV[0][index][0], iAllPoints->_pointsUV[0][index][1], paramTolerance) )
        //        || Common::Failed( _data._uvboxes[1].ContainParam(iAllPoints->_pointsUV[1][index][0], iAllPoints->_pointsUV[1][index][1], paramTolerance) ) )
        //        return Common::Success;

        //    Math::Vector3D prevPoint = iAllPoints->_centralPoints[index];

        //    double step = mult * ( endParam - boundParam );
        //    if (step > 0)
        //    {
        //        step *= 0.5;

        //        bool find = false;
        //        double param = boundParam + mult*step;
        //        double goodParam = 0.0;

        //        TripleArray goodTriple;

        //        int count, countEnd = iStart ? iAllPoints->_counts.front() : iAllPoints->_counts.back();

        //        while (step > paramTolerance)
        //        {
        //            step *= 0.5;
        //            const TripleArray& triple = GetTripleArray(param);
        //            count = triple._count;
        //            bool ok = (count == countEnd);

        //            if (ok)
        //            {
        //                find = true;
        //                goodParam = param;
        //                goodTriple = triple;
        //                param -= mult*step;
        //            } 
        //            else
        //                param += mult*step;

        //            bool fl = false;

        //            if (count > 0)
        //            {
        //                Math::Vector3D centralPoint, tangent;
        //                Math::Vector2D UV1, UV2;

        //                GetNearestPoint(triple, iStart ? iAllPoints->_centralPoints.front() : iAllPoints->_centralPoints.back(), centralPoint, tangent, UV1, UV2);

        //                if ( Common::Failed( _data._uvboxes[0].ContainParam(UV1[0], UV1[1], paramTolerance) )
        //                    || Common::Failed( _data._uvboxes[1].ContainParam(UV2[0], UV2[1], paramTolerance) ) )
        //                    fl = true;

        //                fl = fl || (centralPoint.DistanceTo(prevPoint) < _context->GetLinearPrecision());
        //                prevPoint = centralPoint;
        //            }

        //            if (fl)
        //                break;
        //        } // while


        //        if (find && goodParam != endParam)
        //        {                            
        //            Math::Vector3D goodCentralPoint, goodTangent;
        //            Math::Vector2D goodUV1, goodUV2;

        //            GetNearestPoint(goodTriple, iStart ? iAllPoints->_centralPoints.front() : iAllPoints->_centralPoints.back(), goodCentralPoint, goodTangent, goodUV1, goodUV2);

        //            ProjectCurveOnSurfaceUtils::CheckUVPoint(_data._surfaces[0], iAllPoints->_pointsUV[0][index], goodUV1 );
        //            ProjectCurveOnSurfaceUtils::CheckUVPoint(_data._surfaces[1], iAllPoints->_pointsUV[1][index], goodUV2 );

        //            if (std::abs(boundParam - goodParam) > _context->GetLinearPrecision())
        //            {
        //                if (iStart)
        //                {
        //                    iAllPoints->_params.insert(iAllPoints->_params.begin(), goodParam);
        //                    iAllPoints->_centralPoints.insert(iAllPoints->_centralPoints.begin(), goodCentralPoint);

        //                    iAllPoints->_tangents.insert(iAllPoints->_tangents.begin(), goodTangent);

        //                    iAllPoints->_pointsUV[0].insert(iAllPoints->_pointsUV[0].begin(), goodUV1);
        //                    iAllPoints->_pointsUV[1].insert(iAllPoints->_pointsUV[1].begin(), goodUV2);
        //                    iAllPoints->_counts.insert(iAllPoints->_counts.begin(), countEnd);
        //                }
        //                else
        //                {
        //                    iAllPoints->_params.push_back(goodParam);
        //                    iAllPoints->_centralPoints.push_back(goodCentralPoint);

        //                    iAllPoints->_tangents.push_back(goodTangent);

        //                    iAllPoints->_pointsUV[0].push_back(goodUV1);
        //                    iAllPoints->_pointsUV[1].push_back(goodUV2);
        //                    iAllPoints->_counts.push_back(countEnd);
        //                }
        //            }
        //            else
        //            {
        //                iAllPoints->_params[index] = goodParam;
        //                iAllPoints->_centralPoints[index] = goodCentralPoint;

        //                iAllPoints->_tangents[index] = goodTangent;

        //                iAllPoints->_pointsUV[0][index] = goodUV1;
        //                iAllPoints->_pointsUV[1][index] = goodUV2;
        //                iAllPoints->_counts[index] = countEnd;
        //            } 
        //        }

        //    }

        //    return Common::Success;
        //}

        //Common::Result VRD::SplitCurves( AllPointsPtr iAllPoints, std::vector<double>& splitParams, std::vector<AllPointsPtr>& newCurves )
        //{
        //    splitParams.insert(splitParams.begin(), iAllPoints->_params.front());
        //    splitParams.push_back(iAllPoints->_params.back());

        //    for (std::size_t i=1; i<splitParams.size(); ++i)
        //    {
        //        bool f1 = i>1;
        //        bool f2 = i+1 < splitParams.size();

        //        std::size_t start = (f1)? std::lower_bound(iAllPoints->_params.begin(), iAllPoints->_params.end(), splitParams[i-1]) - iAllPoints->_params.begin() : 0;
        //        std::size_t end = (f2)? std::lower_bound(iAllPoints->_params.begin(), iAllPoints->_params.end(), splitParams[i]) - iAllPoints->_params.begin() : iAllPoints->_params.size();

        //        if (end<=start)
        //            continue;                
        //        
        //        AllPointsPtr apPtr(new AllPoints(f1 ? splitParams[i-1] : 0.0));
        //        _allCurves.push_back(apPtr);

        //        apPtr->_counts.assign(iAllPoints->_counts.begin() + start, iAllPoints->_counts.begin() + end);
        //        apPtr->_centralPoints.assign(iAllPoints->_centralPoints.begin() + start, iAllPoints->_centralPoints.begin() + end);

        //        apPtr->_tangents.assign(iAllPoints->_tangents.begin() + start, iAllPoints->_tangents.begin() + end);

        //        apPtr->_params.assign(iAllPoints->_params.begin() + start, iAllPoints->_params.begin() + end);
        //        apPtr->_pointsUV[0].assign(iAllPoints->_pointsUV[0].begin() + start, iAllPoints->_pointsUV[0].begin() + end);
        //        apPtr->_pointsUV[1].assign(iAllPoints->_pointsUV[1].begin() + start, iAllPoints->_pointsUV[1].begin() + end);

        //        apPtr->_bounds[1] = f2 ? splitParams[i] : 0.0;

        //        newCurves.push_back(apPtr);
        //    }

        //    return Common::Success;
        //}

        //Common::Result VRD::GetSplitAddParams(AllPointsPtr iAllPoints, double uTolerance, std::vector<double>& splitParams, std::vector<double>& addParams )
        //{
        //    std::vector<double>& params = iAllPoints->_params;
        //    std::vector<int>& counts = iAllPoints->_counts;
        //    std::vector<Math::Vector3D>& centralPoints = iAllPoints->_centralPoints;

        //    Common::Result result;                        
        //    
        //    Math::Vector3D pointCenter, tangent;
        //    Math::Vector2D pointFirst, pointSecond;

        //    CurveTriple cTr;

        //    std::vector<double> checkParams;
        //    std::vector<std::size_t> indicies;

        //    for (std::size_t i=1; i<params.size(); ++i)
        //    {
        //        if (params[i]-params[i-1] > uTolerance )
        //        {
        //            double param = ( params[i-1] + params[i] ) * 0.5;

        //            result = AddTriple(param, iAllPoints, iAllPoints->_pointsUV[0][i-1], iAllPoints->_pointsUV[1][i-1], cTr );
        //            
        //            if (Common::Failed(result))
        //            {
        //                checkParams.push_back(param);
        //                indicies.push_back(i);
        //            }
        //            else
        //            {
        //                //const TripleArray& tripleArr = GetTripleArray(param);
        //                //GetNearestPoint(tripleArr, (centralPoints[i-1] + centralPoints[i])*0.5, pointCenter, tangent, pointFirst, pointSecond);
        //                //double aaa = pointCenter.DistanceTo(cTr._triple._pointCenter);
        //                //if ( aaa > _data._tolerance)
        //                //{
        //                //    int j=0;

        //                //    Math::Vector3D tpoint, tpoint2;
        //                //    _data._surfaces[0]->EvaluatePoint(_context, pointFirst[0], pointFirst[1], tpoint);
        //                //    _data._surfaces[1]->EvaluatePoint(_context, pointSecond[0], pointSecond[1], tpoint2);

        //                //    double dist0 = tpoint.DistanceTo(pointCenter);
        //                //    double dist1 = tpoint2.DistanceTo(pointCenter);

        //                //    _data._surfaces[0]->EvaluatePoint(_context, cTr._triple._pointsUV[0][0], cTr._triple._pointsUV[0][1], tpoint);
        //                //    _data._surfaces[1]->EvaluatePoint(_context, cTr._triple._pointsUV[1][0], cTr._triple._pointsUV[1][1], tpoint2);

        //                //    dist0 = tpoint.DistanceTo(cTr._triple._pointCenter);
        //                //    dist1 = tpoint2.DistanceTo(cTr._triple._pointCenter);
        //                //}

        //                double dist;
        //                result = GetMaxDistance(iAllPoints, param, cTr._triple._pointCenter, cTr._triple._pointsUV[0], cTr._triple._pointsUV[1], dist);
        //                if (Common::Failed(result))
        //                    return result;

        //                if (dist > _data._tolerance)
        //                {
        //                    //addParams.push_back(param);
        //                    checkParams.push_back(param);
        //                    indicies.push_back(i);
        //                } 
        //            }
        //        }
        //    } // for

        //    result = CacheTriples(checkParams);
        //    if (Common::Failed(result))
        //        return result;

        //    for (std::size_t i=0; i<checkParams.size(); ++i)
        //    {
        //        bool needSplit = false;

        //        //double par = checkParams[i];

        //        const TripleArray& tripleArr = GetTripleArray(checkParams[i]);

        //        if (tripleArr._count < counts[indicies[i]-1] && tripleArr._count < counts[indicies[i]])
        //        {
        //            needSplit = Check(iAllPoints, tripleArr);
        //        }

        //        if (needSplit)
        //        {
        //            splitParams.push_back(checkParams[i]);
        //        }
        //        else
        //        {
        //            GetNearestPoint(tripleArr, (centralPoints[indicies[i]-1] + centralPoints[indicies[i]])*0.5, pointCenter, tangent, pointFirst, pointSecond);

        //            double dist;
        //            result = GetMaxDistance(iAllPoints, checkParams[i], pointCenter, pointFirst, pointSecond, dist);
        //            if (Common::Failed(result))
        //                return result;

        //            if (dist > _data._tolerance)
        //            {
        //                addParams.push_back(checkParams[i]);
        //            }    
        //        }
        //    }

        //    std::sort(addParams.begin(), addParams.end());
        //    std::sort(splitParams.begin(), splitParams.end());

        //    return Common::Success;
        //}

        //Common::Result VRD::FilterReport( BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport )
        //{
        //    Common::Result result;

        //    Math::Vector3D tangent1, tangent2;
        //    Interval interval;
        //    double param;

        //    const CurvePtr& controlCurve = _data._controlCurve;

        //    for (std::size_t i=0; i < oReport._spineCurves.size(); ++i)
        //    {
        //        const CurvePtr& curve = oReport._spineCurves[i];
        //        curve->GetInterval(interval);

        //        param = interval.GetStart();

        //        controlCurve->EvaluateTangent(_context, param, tangent1);
        //        curve->EvaluateTangent(_context, param, tangent2);

        //        if (tangent1 * tangent2 < 0.0)
        //        {
        //            oReport._spineCurves.erase(oReport._spineCurves.begin()+i);
        //            oReport._linkageCurves[0].erase(oReport._linkageCurves[0].begin()+i);
        //            oReport._linkageCurves[1].erase(oReport._linkageCurves[1].begin()+i);

        //            --i;
        //        }
        //    }

        //    if (_data._uvboxesSet /*&& false*/)
        //    {
        //        std::vector<CurvePtr> centralCurves(oReport._spineCurves);
        //        std::vector<ParametricCurvePtr> linkageCurves[2] = { oReport._linkageCurves[0], oReport._linkageCurves[1]};

        //        oReport._linkageCurves[0].clear();
        //        oReport._linkageCurves[1].clear();
        //        oReport._spineCurves.clear();
        //        
        //        for (std::size_t i=0; i <centralCurves.size(); ++i)
        //        {
        //            std::vector<Interval> badIntervals, bInt;

        //            for (int t=0; t<2; ++t)
        //            {                    
        //                ProjectCurveOnSurfaceUtils::FindUVCurveBadIntervals(_context, linkageCurves[t][i]->GetUVCurve()->AsNURBSCurve(), _data._uvboxes[t], bInt );
        //                badIntervals.insert(badIntervals.end(), bInt.begin(), bInt.end());
        //            }

        //            std::vector<double> parameters;
        //            parameters.reserve(2*badIntervals.size() + 2);

        //            for (std::size_t t=0; t < badIntervals.size(); ++t)
        //            {
        //                parameters.push_back(badIntervals[t][0]);
        //                parameters.push_back(badIntervals[t][1]);
        //            }

        //            const CurvePtr& curve = centralCurves[i];
        //            curve->GetInterval(interval);

        //            parameters.push_back(interval[0]);
        //            parameters.push_back(interval[1]);

        //            std::sort(parameters.begin(), parameters.end());                 

        //            for (std::size_t t=1; t<parameters.size(); ++t)
        //            {
        //                if (parameters[t] - parameters[t-1] < _context->GetLinearPrecision())
        //                    continue;

        //                double param = (parameters[t-1] + parameters[t])*0.5;

        //                bool bad = false;
        //                for (std::size_t l = 0; l < badIntervals.size(); ++l )
        //                {
        //                    if (Common::Succeeded(badIntervals[l].ContainParam(param, 0.0)))
        //                    {
        //                        bad = true;
        //                        break;
        //                    }
        //                }

        //                if (bad)
        //                    continue;

        //                Interval newInterval(parameters[t-1], parameters[t]);

        //                oReport._spineCurves.push_back(centralCurves[i]);
        //                oReport._linkageCurves[0].push_back(linkageCurves[0][i]);
        //                oReport._linkageCurves[1].push_back(linkageCurves[1][i]);

        //                if ( Common::Failed( interval.IsEqualInterval( newInterval, interval.GetLinearPrecision(_context) ) ) )
        //                {
        //                    NURBSCurvePtr nCurve;
        //                    result = oReport._spineCurves.back()->AsNURBSCurve()->Copy(_context, newInterval, nCurve);
        //                    if (Common::Failed(result))
        //                        return result;
        //                    oReport._spineCurves.back() = nCurve;

        //                    result = oReport._linkageCurves[0].back()->GetUVCurve()->AsNURBSCurve()->Copy(_context, newInterval, nCurve);
        //                    if (Common::Failed(result))
        //                        return result;
        //                    ParametricCurve::Create(_context, _data._surfaces[0], nCurve, oReport._linkageCurves[0].back());

        //                    ExportToGrapherUtils::OutputCurveToFile(_context, nCurve, 100, "uvCurve.txt");
        //                    ExportToGrapherUtils::OutputUVBoxToFile(_context, _data._uvboxes[0], "ubBox.txt");

        //                    result = oReport._linkageCurves[1].back()->GetUVCurve()->AsNURBSCurve()->Copy(_context, newInterval, nCurve);
        //                    if (Common::Failed(result))
        //                        return result;
        //                    ParametricCurve::Create(_context, _data._surfaces[1], nCurve, oReport._linkageCurves[1].back());


        //                    ExportToGrapherUtils::OutputCurveToFile(_context, nCurve, 100, "uvCurve.txt");
        //                    ExportToGrapherUtils::OutputUVBoxToFile(_context, _data._uvboxes[0], "ubBox.txt");
        //                }
        //            } // parameters, m
        //        } // centralCurves, i
        //    }

        //    {
        //        std::vector<CurvePtr> centralCurves(oReport._spineCurves);
        //        std::vector<ParametricCurvePtr> linkageCurves1(oReport._linkageCurves[0]);
        //        std::vector<ParametricCurvePtr> linkageCurves2(oReport._linkageCurves[1]);

        //        std::vector<double> parameters;
        //        parameters.reserve(2*oReport._spineCurves.size());

        //        for (std::size_t i=0; i < oReport._spineCurves.size(); ++i)
        //        {
        //            const CurvePtr& curve = oReport._spineCurves[i];
        //            curve->GetInterval(interval);

        //            parameters.push_back(interval[0]);
        //            parameters.push_back(interval[1]);
        //        }

        //        std::sort(parameters.begin(), parameters.end());

        //        Math::Vector3D centralPoint, controlPoint;

        //        oReport._linkageCurves[0].clear();
        //        oReport._linkageCurves[1].clear();
        //        oReport._spineCurves.clear();

        //        std::vector<Interval> intervals;

        //        for (std::size_t i=1; i<parameters.size(); ++i)
        //        {
        //            if (parameters[i] - parameters[i-1] < _context->GetLinearPrecision())
        //                continue;

        //            double param = (parameters[i-1] + parameters[i])*0.5;

        //            controlCurve->EvaluatePoint(_context, param, controlPoint );

        //            int index = -1;
        //            double dist = 1e6;

        //            for (std::size_t l=0; l < centralCurves.size(); ++l)
        //            {
        //                const CurvePtr& curve = centralCurves[l];
        //                curve->GetInterval(interval);

        //                if (Common::Failed(interval.ContainParam(param, interval.GetLinearPrecision(_context))))
        //                    continue;

        //                curve->EvaluatePoint(_context, param, centralPoint);

        //                double locDist = centralPoint.DistanceTo(controlPoint);
        //                if (locDist < dist)
        //                {
        //                    index = (int)l;
        //                    dist = locDist;
        //                }
        //            }

        //            if (index >= 0)
        //            {
        //                if (oReport._spineCurves.empty() || oReport._spineCurves.back() != centralCurves[index])
        //                {
        //                    oReport._spineCurves.push_back(centralCurves[index]);
        //                    oReport._linkageCurves[0].push_back(linkageCurves1[index]);
        //                    oReport._linkageCurves[1].push_back(linkageCurves2[index]);
        //                    intervals.push_back(Interval(parameters[i-1], parameters[i]));
        //                }
        //                else
        //                {
        //                    intervals.back()[1] = parameters[i];
        //                }
        //            }
        //        }

        //        for (std::size_t i=0; i<intervals.size(); ++i)
        //        {
        //            oReport._spineCurves[i]->GetInterval(interval);
        //            if ( Common::Failed( interval.IsEqualInterval( intervals[i], interval.GetLinearPrecision(_context) ) ) )
        //            {
        //                NURBSCurvePtr nCurve;
        //                result = oReport._spineCurves[i]->AsNURBSCurve()->Copy(_context, intervals[i], nCurve);
        //                if (Common::Failed(result))
        //                    return result;
        //                
        //                oReport._spineCurves[i] = nCurve;

        //                oReport._linkageCurves[0][i]->GetUVCurve()->AsNURBSCurve()->Copy(_context, intervals[i], nCurve);

        //                result = ParametricCurve::Create(_context, _data._surfaces[0], nCurve, oReport._linkageCurves[0][i]);
        //                if (Common::Failed(result))
        //                    return result;

        //                oReport._linkageCurves[1][i]->GetUVCurve()->AsNURBSCurve()->Copy(_context, intervals[i], nCurve);

        //                result = ParametricCurve::Create(_context, _data._surfaces[1], nCurve, oReport._linkageCurves[1][i]);
        //                if (Common::Failed(result))
        //                    return result;
        //            }
        //        }
        //    }

        //    //if (false)
        //    for (int t=0;t<2;++t)
        //    {
        //        if (_sourceSurfaces[t] && _sourceSurfaces[t] != _data._surfaces[t])
        //        {
        //            for (std::size_t i=0; i<oReport._linkageCurves[t].size(); ++i)
        //            {
        //                result = ParametricCurve::Create(_context, _sourceSurfaces[t], oReport._linkageCurves[t][i]->GetUVCurve(), oReport._linkageCurves[t][i]);
        //                if (Common::Failed(result))
        //                    return result;    
        //            }
        //        }
        //    }
        //    
        //    //ExportToGrapherUtils::OutputCurveToFile(_context, oReport._linkageCurves[0][0]->GetUVCurve(), 1000, "uvCurve1.txt");
        //    //ExportToGrapherUtils::OutputCurveToFile(_context, oReport._linkageCurves[1][0]->GetUVCurve(), 1000, "uvCurve2.txt");

        //    return Common::Success;
        //}

        //Common::Result VRD::FindGap( const std::vector<double>& iParams, const std::vector<Math::Vector2D>& iPoints, const ParametricCurvePtr& iCurve, std::vector<double>& oParams )
        //{
        //    const SurfacePtr& surf = iCurve->GetSurface();

        //    Common::Result result;

        //    std::vector<Math::Vector3D> points3d(iPoints.size());
        //    for (std::size_t i=0; i<iPoints.size(); ++i)
        //    {
        //        result = surf->EvaluatePoint(_context, iPoints[i][0], iPoints[i][1], points3d[i]);
        //        if (Common::Failed(result))
        //            return result;
        //    }

        //    NURBSCurvePtr nCurve;
        //    int degree = points3d.size() > 3 ? 3 : (int)points3d.size()-1;

        //    result = NURBSInterpolationBuilder::CreateCurveLocalInt(_context, points3d, std::vector<Math::Vector3D>(), iParams, nCurve, degree, false, true, NURBSInterpolationBuilder::G1Continious);
        //    if (Common::Failed(result))
        //        return result;

        //    const int N = 1000;

        //    //ExportToGrapherUtils::OutputCurveToFile(_context, iCurve, 100, "Curve.txt");
        //    //ExportToGrapherUtils::OutputCurveToFile(_context, nCurve, 100, "nCurve.txt");

        //    Math::Vector3D point, point2;
        //    //int index = -1;
        //    double maxDist = 0.0;
        //    double paramMax = iParams.front() -1.0;

        //    for (int i=1; i<N; ++i)
        //    {
        //        double param = iParams.front() + (iParams.back() - iParams.front())*i/N;

        //        iCurve->EvaluatePoint(_context, param, point);
        //        nCurve->EvaluatePoint(_context, param, point2);

        //        double dist = point.DistanceTo(point2);
        //        if (dist > maxDist)
        //        {
        //            maxDist = dist;
        //            //index = i;
        //            paramMax = param;
        //        }
        //    }

        //    if (paramMax > iParams.front())
        //    {
        //        oParams.push_back(paramMax);
        //    }

        //    return Common::Success;
        //}

        //Common::Result VRD::FindGap2( AllPointsPtr allPoints, int N, TripleArray &iTripleArr )
        //{
        //    RadiusDiskTools diskTools(_data._surfaces[0], _data._surfaces[1], _data._controlCurve, _data._controlCurveInterval, _data._offsetLaws[0], _data._offsetLaws[1], 
        //        _data._orientations[0], _data._orientations[1], _data._tolerance);

        //    diskTools.SetUVBoxes(_data._uvboxes[0], _data._uvboxes[1]);

        //    Math::Vector2D inUV[2] = {allPoints->_pointsUV[0].back(), allPoints->_pointsUV[1].back()};
        //    Math::Vector2D outUV[2];
        //    Math::Vector3D point;

        //    Common::Result result = Common::Success;

        //    for(int t=1;t<N;++t)
        //    {
        //        double param = allPoints->_params.back() + (iTripleArr._param - allPoints->_params.back())*t/(N-1);

        //        result = diskTools.IterateToCircleTangentPoint(_context, param, inUV, point, outUV );
        //        if (Common::Failed(result))
        //            break;
        //    }

        //    return result;
        //}

        //void VRD::AllPoints::AddTriple(Common::Context* iContext, const Triple& iTriple, double iParam, int iCount )
        //{
        //    if (_params.empty() || iParam > _params.back() + iContext->GetLinearPrecision())
        //    {
        //        _centralPoints.push_back(iTriple._pointCenter);
        //        _tangents.push_back(iTriple._tangent);

        //        _pointsUV[0].push_back(iTriple._pointsUV[0]);
        //        _pointsUV[1].push_back(iTriple._pointsUV[1]);
        //        _params.push_back(iParam);
        //        _counts.push_back(iCount);
        //    }
        //}

        //void VRD::AllPoints::InsertTriple( std::size_t index, const Triple& iTriple, double iParam, int iCount, 
        //    const BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesData& iData )
        //{
        //    _centralPoints.insert(_centralPoints.begin() + index, iTriple._pointCenter);

        //    _tangents.insert(_tangents.begin() + index, iTriple._tangent);

        //    _pointsUV[0].insert(_pointsUV[0].begin() + index, iTriple._pointsUV[0]);
        //    _pointsUV[1].insert(_pointsUV[1].begin() + index, iTriple._pointsUV[1]);
        //    _params.insert(_params.begin() + index, iParam);
        //    _counts.insert(_counts.begin() + index, iCount);

        //    if (index+1 < _pointsUV[0].size())
        //    {
        //        ProjectCurveOnSurfaceUtils::CheckUVPoint(iData._surfaces[0], _pointsUV[0][index+1], _pointsUV[0][index] );
        //        ProjectCurveOnSurfaceUtils::CheckUVPoint(iData._surfaces[1], _pointsUV[1][index+1], _pointsUV[1][index] );
        //    }
        //    else if (index > 0)
        //    {
        //        ProjectCurveOnSurfaceUtils::CheckUVPoint(iData._surfaces[0], _pointsUV[0][index-1], _pointsUV[0][index] );
        //        ProjectCurveOnSurfaceUtils::CheckUVPoint(iData._surfaces[1], _pointsUV[1][index-1], _pointsUV[1][index] );
        //    }

        //    //if (iData._uvboxesSet && false)
        //    //{
        //    //    _pointsUV[0][index] = iData._uvboxes[0].DrawUVPointInUVBox(_pointsUV[0][index]);
        //    //    _pointsUV[1][index] = iData._uvboxes[1].DrawUVPointInUVBox(_pointsUV[1][index]);
        //    //}
        //}


        //VRD::TripleArray::TripleArray() : _count(0), _param(0.0)
        //{
        //}

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