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
#include "../TValueParamApproximation.h"
#include "Generators/Blending/Geometry/BlendSurfaceByVariableRadiusDisk.h"
//#include "../TValueDoubleApproximation.h"
#include "VRDobject.h"
#include "VRDApproximation.h"

namespace RGK
{
    namespace Geometry
    {
        /// Генератор поверхности сглаживания через 2 закона радиус-смещения
        class DLLLOCAL VRDApprox : public TValueParamApproximation<Triple, double>
        {
        public:
            using TValueParamApproximation<Triple, double>::operator=;

            VRDApprox(Common::Context*iContext, const BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesData& iData, 
                std::shared_ptr<VRDObject> iObject) : TValueParamApproximation<Triple, double>(iObject) 
            {
                _context = iContext;
                _data = iData;
            }

//DOM-IGNORE-BEGIN
   
            //protected:

            virtual void MakeApproximation() override;

            Common::Context *_context;
            BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesData _data;

            //DOM-IGNORE-END
        };
    }
}

