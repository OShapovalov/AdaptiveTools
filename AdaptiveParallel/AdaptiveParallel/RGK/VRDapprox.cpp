#include "VRDapprox.h"
#include "Math\NURBS\NURBSInterpolationBuilder.h"
#include "Geometry\Curves\ProjectCurveOnSurfaceUtils.h"
#include "VRDApproximation.h"

namespace RGK
{
    namespace Geometry
    {
//#define MIN_COUNT 100 /*10000*/
//#define MAX_POINTS 1000
//
//#define GAP_COUNT 5
//#define EXT_PARAM_MIN 0.01
//#define EXT_PARAM_MAX 0.5


        void VRDApprox::MakeApproximation()
        {
            _approxObject->MakeApprox(_params, _values); 
        }


    }
}