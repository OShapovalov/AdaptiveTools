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
        class DLLLOCAL VRDObject : TValueParamObject<TripleArray, double>
        {
        public:

            VRDObject(){}

            VRDObject & operator = (VRDObject const & /*num*/)
            {
                return *this;
            }

            virtual TripleArray Evaluate(const double& iParam) const override
            {
                TripleArray arr;
                return arr;
            }

            virtual void GetIntervals(std::vector<TplInterval>& oIntervals) const override
            {
                oIntervals.clear();
                oIntervals.push_back(TplInterval(_data._controlCurveInterval[0],_data._controlCurveInterval[1]));
            }

//DOM-IGNORE-BEGIN
   
     private:

            Common::Context* _context;

            BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesData _data;

            bool _extend;

            NURBSSurfacePtr _sourceSurfaces[2];
            //DOM-IGNORE-END
        };
    }
}

