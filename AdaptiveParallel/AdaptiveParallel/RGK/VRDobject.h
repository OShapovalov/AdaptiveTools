// VRDObject.h
//
// Данный файл является частью библиотеки классов 3D ядра RGK
//(c)Авторское право 2012-2013 АО "Топ Системы". Все права защищены.
//
// Контактная информация:
// rgk@topsystems.ru
// http://www.topsystems.ru
//
/////////////////////////////////////////////////////////////////////////////
// Содержание файла: RGK::Geometry::VRDObject
// Автор: Шаповалов О.В.
// Дата создания: 31.07.2012
// Последнее изменение: Шаповалов О.В.
// Дата последнего изменения: 31.07.2012
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../TplInterval.h"
#include "decl.h"
#include "forward.h"
#include "Geometry/Curves/Curve.h"
#include "Geometry/Surfaces/Surface.h"
#include "Geometry/Surfaces/NURBSSurface.h"
#include "Generators/Law.h"
#include "Math/Vector2D.h"
#include "Geometry/Surfaces/CircularBlendingOffsetSurface.h"
#include "../TValueDoubleObject.h"
#include "Generators/Blending/Geometry/BlendSurfaceByVariableRadiusDisk.h"

namespace RGK
{
    namespace Geometry
    {
        /// Генератор поверхности сглаживания через 2 закона радиус-смещения
        class DLLLOCAL VRDObject : public TValueParamObject<Triple, double>
        {
        public:

            VRDObject( Common::Context* iContext, const BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesData& iData):
                _context(iContext), _data(iData) {}

            using TValueParamObject<Triple, double>::operator=;

            virtual Triple Evaluate(const double& iParam) const override
            {
                TripleArray trArr;
                CreatePoints(iParam, trArr);

                if (trArr._arr.empty())
                    return Triple();

                return trArr._arr.front();
            }

            virtual void GetIntervals(std::vector<TplInterval>& oIntervals) const override
            {
                oIntervals.clear();
                oIntervals.push_back(TplInterval(_data._controlCurveInterval[0],_data._controlCurveInterval[1]));
            }

            //virtual void MakeApprox( std::vector<double>& params, const std::vector<Triple>& triples) override;

//DOM-IGNORE-BEGIN
   
     private:

         static RGK::Common::Result GetOffsetSide( Common::Context* iContext, const CurvePtr& curve, const Interval& interval, const SurfacePtr& iSurface, 
             double iTolerance, bool iOrientation, const Math::Vector3D& iPlaneNormal, Curve::EquidistantOnSurfaceData::OffsetSide& offsetSide );

         Common::Result GetEquidistantCurves( const SurfacePtr& iSurface, bool iOrientation, const RGK::Geometry::PlanePtr& plane, double iOffset, const RGK::Geometry::Surface::IntersectSurfaceReport& intersectReport, 
             std::vector<CurvePtr>& eqcurves, std::vector<Interval>& intervals ) const;

         Common::Result ProjectPoint( const Math::Vector3D& point, const CurvePtr& iCurve, const Interval& iInterval, const SurfacePtr& iSurface, Math::Vector2D& oPointUV ) const;

         Common::Result CreatePoints(double iParam, TripleArray& oTriples) const;

         Common::Context* _context;

         BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesData _data;

         bool _extend;

         NURBSSurfacePtr _sourceSurfaces[2];
            //DOM-IGNORE-END
        };
    }
}

