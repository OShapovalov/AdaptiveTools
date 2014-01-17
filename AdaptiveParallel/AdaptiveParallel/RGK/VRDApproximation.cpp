#include "VRDApproximation.h"
#include "Math\NURBS\NURBSInterpolationBuilder.h"
#include "Geometry\Curves\ProjectCurveOnSurfaceUtils.h"

namespace RGK
{
    namespace Geometry
    {
        RGK::Geometry::Triple RGK::Geometry::VRDApproximation::Evaluate( const double& iParam ) const 
        {
            Triple trArr;
            _centralCurve->EvaluatePoint(_context, iParam, trArr._pointCenter);

            Math::Vector3D temp[2];

            _pcurves[0]->EvaluatePoint(_context, iParam, temp[0]);
            _pcurves[1]->EvaluatePoint(_context, iParam, temp[1]);

            trArr._pointsUV[0] = Math::Vector2D(temp[0]);
            trArr._pointsUV[1] = Math::Vector2D(temp[1]);

            return trArr;
        }

        void RGK::Geometry::VRDApproximation::MakeApprox( std::vector<double>& params, const std::vector<Triple>& triples )
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

                ProjectCurveOnSurfaceUtils::CreateByUVPointsG1(_context, _data._surfaces[i], uvbox, pointsUV[i], params, 1e-10, _pcurves[i]);
            }

            int degree = std::min(3, (int)params.size()-1);

            NURBSInterpolationBuilder::CreateCurveLocalInt(_context, centralPoints, std::vector<Math::Vector3D>(), params, 
                _centralCurve, degree, false, true, NURBSInterpolationBuilder::G1Continious);
        }


    }
}

