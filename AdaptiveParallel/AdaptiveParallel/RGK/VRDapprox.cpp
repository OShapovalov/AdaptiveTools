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
            _approxObject = std::make_shared<VRDApproximation>(_context, _data, _params,_values,_dimensions,_object);
        }

        void VRDApprox::FillReport( BlendSurfaceByVariableRadiusDisk::MakeLinkageCurvesReport& oReport )
        {
            std::shared_ptr<VRDApproximation> apprObj = std::dynamic_pointer_cast<VRDApproximation>(_approxObject);
            oReport._spineCurves.push_back(apprObj->GetCentralCurve());
            oReport._linkageCurves[0].push_back(apprObj->GetLinkageCurve1());
            oReport._linkageCurves[1].push_back(apprObj->GetLinkageCurve2());
        }


    }
}