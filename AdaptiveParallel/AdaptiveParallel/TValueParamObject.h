#pragma once
#include <memory>
#include "TplInterval.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "TParamObjectPolicy.h"

template< class TValue, class TParam >
class TValueParamObject : 
	public std::enable_shared_from_this< TValueParamObject<TValue,TParam> >, public TParamObjectPolicy<TParam>
{

public:

	virtual TValue Evaluate(const TParam& iParam) const = 0;

	virtual void GetParams(std::size_t N, std::vector<TParam>& oParams, 
		std::vector<std::size_t>& oDimensions)
	{
		// получить интервалы и заполнить
		std::vector<TplInterval> intervals;
		GetIntervals(intervals);

		std::size_t NDim = intervals.size();

		std::vector<std::vector<double>> allParams(NDim);

		std::size_t linearN = (std::size_t)pow((double)N,1.0/(double)NDim);

		for (std::size_t i = 0; i < NDim; ++i)
		{
			GetParamsForInterval(intervals[i], linearN, allParams[i]);
		}

		oParams.clear();
		TParamObjectPolicy::FillParams(allParams, oParams);
		oDimensions.clear();
		oDimensions.resize(NDim, linearN);
	}

	virtual void GetIntervals(std::vector<TplInterval>& oIntervals) const = 0;

	virtual TplInterval GetInterval(int iDim = 0) const
	{
		std::vector<TplInterval> intervals;
		GetIntervals(intervals);
		return intervals[iDim];
	}

    //virtual void MakeApprox(std::vector<TParam>& params, const std::vector<TValue>& values) = 0;

protected:
	
	virtual ~TValueParamObject(){};

private:

	static void GetParamsForInterval(const TplInterval& iInterval, std::size_t N, std::vector<double>& oParams)
	{
		double start = iInterval.GetStart();
		double end = iInterval.GetEnd();
		double delta = (end - start) / (N-1);

		oParams.resize(N);

		for (std::size_t i=0; i<N-1; ++i)
		{
			oParams[i] = start;
			start += delta;
		}

		oParams[N-1] = end;
	}
};