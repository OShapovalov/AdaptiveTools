// VRD.h
//
// Данный файл является частью библиотеки классов 3D ядра RGK
//(c)Авторское право 2012-2013 АО "Топ Системы". Все права защищены.
//
// Контактная информация:
// rgk@topsystems.ru
// http://www.topsystems.ru
//
/////////////////////////////////////////////////////////////////////////////
// Содержание файла: RGK::Geometry::VRD
// Автор: Шаповалов О.В.
// Дата создания: 31.07.2012
// Последнее изменение: Шаповалов О.В.
// Дата последнего изменения: 31.07.2012
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "decl.h"
#include "forward.h"
#include "Geometry/Curves/Curve.h"
#include "Geometry/Surfaces/Surface.h"
#include "Geometry/Surfaces/NURBSSurface.h"
#include "Generators/Law.h"
#include "Math/Vector2D.h"
#include "Geometry/Surfaces/CircularBlendingOffsetSurface.h"
#include "VRDObject.h"
#include "Generators/Blending/Geometry/BlendSurfaceByVariableRadiusDisk.h"
#include "VRDapprox.h"
#include <memory>

namespace RGK
{
    namespace Geometry
    {
        /// Генератор поверхности сглаживания через 2 закона радиус-смещения
        class DLLLOCAL VRD
        {
        public:

            VRD(Common::Context* iContext, const BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesData& iData):
              _approx(std::make_shared<VRDApprox>(std::make_shared<VRDObject>(iContext,iData))){}

            Common::Result MakeLinkageCurves( Common::Context* iContext, 
                const BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesData& iData, 
                BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport );

//DOM-IGNORE-BEGIN
   
     private:

            Common::Context* _context;

            //VRDObject _object;
            std::shared_ptr<VRDApprox> _approx;

            BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesData _data;

            //bool _table;

            bool _extend;

            NURBSSurfacePtr _sourceSurfaces[2];

            struct Triple
            {
            public:
                
                Triple(){}

                // output
                Math::Vector3D _pointCenter;
                Math::Vector3D _tangent;
                Math::Vector2D _pointsUV[2];
            };

            struct TripleArray
            {
                explicit TripleArray(double iParam) : _count(0), _param(iParam){}

                inline const Triple& operator[](std::size_t idx) const { return _arr[idx]; };

                inline Triple& operator[](std::size_t idx) { return _arr[idx]; };

                std::vector<Triple> _arr;                
                
                int _count;

                TripleArray();
                
                double _param;

                Common::Result GetNearest( const Math::Vector3D& approxPoint, Math::Vector3D& centralPoint, Math::Vector3D& tangent, Math::Vector2D& pointFirst, Math::Vector2D& pointSecond ) const;
            };

            std::vector< TripleArray > _cachePoints;

            struct AllPoints
            {
            public:

                AllPoints(double prevParam):_created(false) { _bounds[0] = prevParam; }

                void AddTriple(Common::Context* iContext, const Triple& iTriple, double iParam, int iCount);

                void InsertTriple(std::size_t index, const Triple& iTriple, double iParam, int iCount, 
                    const BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesData& iData );

                // output
                std::vector<Math::Vector3D> _centralPoints;
                std::vector<Math::Vector3D> _tangents;
                std::vector<Math::Vector2D> _pointsUV[2];

                std::vector<int> _counts;

                std::vector<double> _params;

                double _bounds[2];

                bool _created;

                NURBSCurvePtr _centralCurve;
                ParametricCurvePtr _pcurves[2];

            private:
                AllPoints(){};
            };

            typedef std::shared_ptr<AllPoints> AllPointsPtr;

            struct CurveTriple
            {
                CurveTriple(){}

                double _param;
                AllPointsPtr _allPoints;
                Triple _triple;
            };

            std::vector< CurveTriple > _cacheTriples;

            std::vector<AllPointsPtr> _allCurves;

            Common::Result AddPoints(double prevParam, TripleArray& iTripleArr, BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport);

            Common::Result FindGap2( AllPointsPtr allPoints, int N, TripleArray &iTripleArr );

            Common::Result InsertAllTriples( const TripleArray& iTripleArr, AllPointsPtr iAllPoints);

            Common::Result FinishAll(BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport);

            Common::Result CreateCurve(AllPointsPtr iAllPoints, BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport);

            bool Check(AllPointsPtr iAllPoints, const TripleArray& iTripleArr) const;
        
            Common::Result _MakeLinkageCurves( BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport );

            //Common::Result CacheMiddleTriples( const std::vector<double>& iParams, double uTolerance);

            //Common::Result CacheTriples( const std::vector<double>& iParams);

            //const TripleArray& GetTripleArray( double param);

            Common::Result GetMaxDistance(AllPointsPtr iAllPoints, double iParam, const Math::Vector3D& pointCenter, 
                const Math::Vector2D& pointFirst, const Math::Vector2D& pointSecond, double& oDist );

            static Common::Result GetToleranceForSurface(Common::Context *iContext, SurfacePtr iSurface, const std::vector<Math::Vector2D>& iUVPoints, double iTolerance, double& oParamTolerance);

            static Common::Result UpdateMaxDistance(Common::Context* iContext, const CurvePtr& iCurve, double iParam, const Math::Vector3D& iPoint, double& ioDist);

            //Common::Result MakeAll(BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport );

            //Common::Result GetInitialParams(int& N, std::vector<double> &params );

            //static Common::Result GetNearestPoint( const TripleArray& triple, const Math::Vector3D& approxPoint, Math::Vector3D& centralPoint, Math::Vector3D& tangent, 
            //    Math::Vector2D& pointFirst, Math::Vector2D& pointSecond );

            //Common::Result RefineEnds( AllPointsPtr iAllPoints, bool iStart );

            //Common::Result SplitCurves( AllPointsPtr iAllPoints, std::vector<double>& splitParams, std::vector<AllPointsPtr>& newCurves );

            //Common::Result GetSplitAddParams( AllPointsPtr iAllPoints, double uTolerance, std::vector<double>& splitParams, std::vector<double>& addParams );

            

            //Common::Result ProjectPoint( const Math::Vector3D& point, const CurvePtr& iCurve, const Interval& iInterval, const SurfacePtr& iSurface, Math::Vector2D& oPointUV );

            //Common::Result FilterReport( BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport );

            //Common::Result FindGap(const std::vector<double>& iParams, const std::vector<Math::Vector2D>& iPoints, const ParametricCurvePtr& iCurve, std::vector<double>& params );
            
            //Common::Result AddTriple(double param, AllPointsPtr iAllpoints, const Math::Vector2D& iUV1, const Math::Vector2D& iUV2, VRD::CurveTriple& oCurveTriple );

            //Common::Result GetTriple(double param, AllPointsPtr iAllpoints, VRD::CurveTriple& oCurveTriple) const;
            //DOM-IGNORE-END
        };
    }
}

