#pragma once

#include "ObjectApproximation.h"
#include "Vector3D.h"

class PlaneApproximation : public ObjectApproximation<tplVector3D,tplVector2D>
{
public:

	PlaneApproximation(const std::vector<tplVector2D>& iParams,
		const std::vector<tplVector3D>& iValues,
		const std::vector<std::size_t>& iDimensions,
		std::shared_ptr<TValueParamObject<tplVector3D,tplVector2D>> iObject) :
	ObjectApproximation<tplVector3D, tplVector2D>(iParams, iValues, iDimensions, iObject)
	{

	}

	virtual tplVector3D Evaluate(const tplVector2D& iParams) const override
	{
		std::size_t i=0, k=0;
		std::size_t N1 = _dimensions[0];
		std::size_t N2 = _dimensions[1];

		while (_params[i*N2+k][0] < iParams[0]) ++i;
		while (_params[i*N2+k][1] < iParams[1]) ++k;

		std::size_t predI = i==0?0:i-1, predK = k==0?0:k-1;

		if (i == 0 && k == 0)
			return _values.front();

		tplVector3D deltaU, deltaV, deltaU1, deltaV1;
		double percentU, percentV;

		if (i>0)
		{
			percentU = (iParams[0] - _params[predI*N2+predK][0]) / (_params[i*N2+predK][0] - _params[predI*N2+predK][0]);
			deltaU = (_values[i*N2+predK] - _values[predI*N2+predK])*percentU;

			deltaU1 = (_values[i*N2+k] - _values[predI*N2+k])*(1-percentU);
			//deltaU = (deltaU+deltaU1)/2.0;
		}

		if (k>0)
		{
			percentV = (iParams[1] - _params[predI*N2+predK][1]) / (_params[predI*N2+k][1] - _params[predI*N2+predK][1]);
			deltaV = (_values[predI*N2+k] - _values[predI*N2+predK])*percentV;

			deltaV1 = (_values[i*N2+k] - _values[i*N2+predK])*(1-percentV);
			//deltaV = (deltaV+deltaV1)/2.0;
		}

		tplVector3D retValue = _values[predI*N2+predK] + deltaU + deltaV;

		tplVector3D retValue1 = _values[i*N2+k] - deltaU1 - deltaV1;

		retValue = (retValue+retValue1)/2.0;

		return retValue;
	}

private:

};