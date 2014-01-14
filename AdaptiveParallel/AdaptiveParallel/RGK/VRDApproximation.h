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
        class DLLLOCAL VRDApproximation : ObjectApproximation<Triple, double>
        {
        public:
            //using TValueParamApproximation<Triple, double>::operator=;

            //VRDApprox(std::shared_ptr<VRDObject> iObject):TValueParamApproximation(iObject){}

            VRDApproximation(const std::vector<double>& iParams,
                const std::vector<Triple>& iValues,
                const std::vector<std::size_t>& iDimensions,
                std::shared_ptr<TValueParamObject<Triple,double>> iObject) : 
            ObjectApproximation(iParams, iValues, iDimensions, iObject)
            {

            }

            virtual Triple Evaluate(const double& iParam) const override
            {
                Triple trArr;
                centralCurve->EvaluatePoint(_context, iParam, trArr._pointCenter);

                Math::Vector3D temp[2];

                pcurves[0]->EvaluatePoint(_context, iParam, temp[0]);
                pcurves[1]->EvaluatePoint(_context, iParam, temp[1]);

                trArr._pointsUV[0] = Math::Vector2D(temp[0]);
                trArr._pointsUV[1] = Math::Vector2D(temp[1]);

                return trArr;
            }

            void VRDApproximation::MakeApprox( std::vector<double>& params, const std::vector<Triple>& triples)
            {
                std::vector<Math::Vector2D> pointsUV[2];
                pointsUV[0].resize(triples.size());
                pointsUV[1].resize(triples.size());

                std::vector<Math::Vector3D> centralPoints(triples.size());

                for (std::size_t i=0; i<triples.size(); ++i)
                {
                    centralPoints[i] = triples[i]._pointCenter;
                    pointsUV[0][i] = triples[i]._pointsUV[0];
                    pointsUV[1][i] = triples[i]._pointsUV[1];
                }

                for (int i=0; i<2; ++i)
                {
                    UVBox uvbox;
                    _data._surfaces[i]->GetUVBox(uvbox);

                    ProjectCurveOnSurfaceUtils::CreateByUVPointsG1(_context, _data._surfaces[i], uvbox, pointsUV[i], params, 1e-10, pcurves[i]);
                }

                int degree = std::min(3, (int)params.size()-1);

                NURBSInterpolationBuilder::CreateCurveLocalInt(_context, centralPoints, std::vector<Math::Vector3D>(), params, 
                    centralCurve, degree, false, true, NURBSInterpolationBuilder::G1Continious);
            }

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

