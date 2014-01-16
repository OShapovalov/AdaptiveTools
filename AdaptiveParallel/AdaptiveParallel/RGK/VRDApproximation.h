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
//#include "../TValueParamApproximation.h"
//#include "Generators/Blending/Geometry/BlendSurfaceByVariableRadiusDisk.h"
//#include "../TValueDoubleApproximation.h"
//#include "VRDobject.h"
#include "..\ObjectApproximation.h"
#include "Generators\Blending\Geometry\BlendSurfaceByVariableRadiusDisk.h"

namespace RGK
{
    namespace Geometry
    {
        /// Генератор поверхности сглаживания через 2 закона радиус-смещения
        class DLLLOCAL VRDApproximation : public ObjectApproximation<Triple, double>
        {
        public:
            //using TValueParamApproximation<Triple, double>::operator=;

            //VRDApprox(std::shared_ptr<VRDObject> iObject):TValueParamApproximation(iObject){}

            //VRDApproximation(const std::vector<double>& iParams,
            //    const std::vector<Triple>& iValues,
            //    const std::vector<std::size_t>& iDimensions,
            //    std::shared_ptr<TValueParamObject<Triple,double>> iObject) : 
            //ObjectApproximation(iParams, iValues, iDimensions, iObject)
            //{

            //}

            virtual Triple Evaluate(const double& iParam) const override;

            void MakeApprox( std::vector<double>& params, const std::vector<Triple>& triples);

//DOM-IGNORE-BEGIN
   
            protected:

            //virtual void MakeApproximation() override;

            //DOM-IGNORE-END

            Common::Context *_context;
            BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesData _data;

            ParametricCurvePtr pcurves[2];
            NURBSCurvePtr centralCurve;
        };
    }
}

