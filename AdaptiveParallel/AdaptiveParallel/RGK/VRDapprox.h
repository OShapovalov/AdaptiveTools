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

namespace RGK
{
    namespace Geometry
    {
        /// Генератор поверхности сглаживания через 2 закона радиус-смещения
        class DLLLOCAL VRDApprox : TValueParamApproximation<Triple, double>
        {
        public:
            using TValueParamApproximation<Triple, double>::operator=;

            VRDApprox(std::shared_ptr<VRDObject> iObject):TValueParamApproximation(iObject){}

//DOM-IGNORE-BEGIN
   
            //protected:

            virtual void MakeApproximation() override;

            //DOM-IGNORE-END
        };
    }
}

