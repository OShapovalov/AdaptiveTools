#pragma once
#include <memory>
#include "Interval.h"
#include "Vector2D.h"
#include "Vector3D.h"

template<class TParam>
class TParamObjectPolicy
{
public:

	virtual void GetCheckParams(const std::vector<TParam>& iParams, const std::vector<std::size_t>& iDimensions, std::vector<TParam>& oParams) = 0;
};

template<>
class TParamObjectPolicy<double>
{
public:

	virtual void GetCheckParams(const std::vector<double>& iParams, const std::vector<std::size_t>& iDimensions, std::vector<double>& oParams)
	{
		std::size_t N = iDimensions[0];
		oParams.resize(N-1);	
		for (std::size_t i=0;i<N-1;++i)
		{
			oParams[i] = (iParams[i] + iParams[i+1]) / 2.0;
		}
	}
};

template<>
class TParamObjectPolicy<Vector2D>
{
public:

	virtual void GetCheckParams(const std::vector<Vector2D>& iParams, const std::vector<std::size_t>& iDimensions, std::vector<Vector2D>& oParams)
	{
		std::size_t N1 = iDimensions[0];
		std::size_t N2 = iDimensions[1];
		oParams.resize((N1-1)*(N2-1));
		for (std::size_t i =0; i<N1-1; ++i)
			for (std::size_t k =0; k<N2-1; ++k)
			{
				oParams[i*(N2-1)+k] = (iParams[i*N2+k] + iParams[(i+1)*N2+(k+1)])/2.0;
			}
	}

};

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
		std::vector<Interval> intervals;
		GetIntervals(intervals);

		std::size_t NDim = intervals.size();

		std::vector<std::vector<double>> allParams(NDim);

		std::size_t linearN = (std::size_t)pow((double)N,1.0/(double)NDim);

		for (std::size_t i = 0; i < NDim; ++i)
		{
			GetParamsForInterval(intervals[i], linearN, allParams[i]);
		}

		oParams.clear();
		FillParams(allParams, oParams);
		oDimensions.clear();
		oDimensions.resize(NDim, linearN);
	}

	virtual void GetIntervals(std::vector<Interval>& oIntervals) const = 0;

	virtual Interval GetInterval(int iDim = 0) const
	{
		std::vector<Interval> intervals;
		GetIntervals(intervals);
		return intervals[iDim];
	}

protected:
	
	virtual ~TValueParamObject(){};

private:

	static void GetParamsForInterval(const Interval& iInterval, std::size_t N, std::vector<double>& oParams)
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

	void FillParams( int rec, const std::vector<std::vector<double>>& allParams, std::vector<TParam>& oParams, std::vector<double>& params) 
	{
		for (std::size_t i=0; i<allParams[rec].size(); ++i )
		{			
			if (rec == allParams.size()-1)
			{
				params.push_back(allParams[rec][i]);
				oParams.push_back( TValue(params) );
				params.pop_back();
			}
			else
			{
				params.push_back(allParams[rec][i]);
				FillParams(rec+1, allParams, oParams, params);
				params.pop_back();
			}
		}
	}

	void FillParams(std::vector<std::vector<double>> allParams, std::vector<double>& oParams ) 
	{			
		oParams.resize(allParams[0].size());
		for (std::size_t i =0; i<allParams[0].size(); ++i)
		{
			oParams[i] = allParams[0][i];
		}
	}

	void FillParams(std::vector<std::vector<double>> allParams, std::vector<Vector2D>& oParams ) 
	{		
		std::size_t N1 = allParams[0].size();
		std::size_t N2 = allParams[1].size();
		oParams.resize(N1*N2);
		for (std::size_t i =0; i<N1; ++i)
		for (std::size_t k =0; k<N2; ++k)
		{
			oParams[i*N2+k] = Vector2D(allParams[0][i],allParams[1][k]);
		}
	}

	void FillParams(std::vector<std::vector<double>> allParams, std::vector<Vector3D>& oParams ) 
	{		
		std::size_t N1 = allParams[0].size();
		std::size_t N2 = allParams[1].size();
		std::size_t N3 = allParams[2].size();
		oParams.resize(N1*N2*N3);
		for (std::size_t i =0; i<N1; ++i)
			for (std::size_t k =0; k<N2; ++k)
				for (std::size_t l =0; l<N3; ++l)
			{
				oParams[i*N2*N3+k*N3+l] = Vector3D(allParams[0][i],allParams[1][k],allParams[2][l]);
			}
	}


};