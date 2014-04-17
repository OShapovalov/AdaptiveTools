#pragma once

//#include <Windows.h>
//#include <stdlib.h>
//#include "AbstractParallel.h"
#include <set>
//#include <vector>
//#include "TValueParamObject.h"
//#include "Statistics.h"
#include <assert.h>
#include "TplInterval.h"
#include "TValueParamObject.h"
#include "ParallelUtils.h"

template< class TValue, class TParam >
class TValueParamApproximation : public TValueParamObject<TValue, TParam>
{
public:

	//TValueParamApproximation(std::shared_ptr<TValueParamObject<TValue,TParam>> iObject) : 
 //     _object(iObject)
 //     {
 //         _pUtils = std::make_shared<ParallelUtils>("TValueParamApprox.ini");
 //     };

    TValueParamApproximation(std::shared_ptr<TValueParamObject<TValue,TParam>> iObject,
        ParallelUtilsPtr iPUtils) : _object(iObject), _pUtils(iPUtils){};

	//static std::shared_ptr<TValueParamApproximation<TValue,TParam>> Create(std::shared_ptr<TValueParamObject<TValue,TParam>> iObject)
	//{
	//	return std::make_shared<TValueParamApproximation<TValue,TParam>>(iObject);
	//}

    virtual void MakeApprox(int n, double iTolerance)
    {
        FillParamsAndValues(n);

        bool isFinished = false;

        int iter = 0;

        while (!isFinished && iter < 10)
        {
            ++iter;

            MakeApproximation();

            isFinished = true;

            std::vector<TParam> params;
            _object->GetCheckParams(_params, _dimensions, params);

            int N = (int)params.size();

            std::vector<bool> needToAdd(N);
            std::vector<TValue> exactValues(N);

            CacheParams(params, exactValues);

            _pUtils->RunInParallel( [&](int i)
            {
                needToAdd[i] = CheckValue(params[i], iTolerance, exactValues[i]);
            }, 0, N);

            if (std::find(needToAdd.begin(),needToAdd.end(),true)!=needToAdd.end())
                isFinished = false;

            Insert(params, needToAdd, exactValues);
        }

    }

protected:

	TValueParamApproximation(){};

	virtual void GetIntervals(std::vector<TplInterval>& oIntervals) const override
	{
		return _object->GetIntervals(oIntervals);
	}

	void Insert(const std::vector<TParam>& params, const std::vector<bool>& needToAdd, const std::vector<TValue>& exactValues)
	{
		if (_dimensions.size() == 1)
		{
			std::size_t N = _dimensions[0];
			int alreadyAdded = 0;
			for (std::size_t i = 0; i < N-1; ++i)
			{
				if (needToAdd[i])
				{
					++alreadyAdded;
					std::size_t index = i + alreadyAdded;
					_params.insert(_params.begin() + index, params[i] );
					_values.insert(_values.begin() + index, exactValues[i] );
				}
			}

			_dimensions[0] += alreadyAdded;
		}
		else if (_dimensions.size() == 2)
		{
			std::size_t& N1 = _dimensions[0];
			std::size_t& N2 = _dimensions[1];

			std::set<std::size_t> colToAdd;

			int alreadyRowAdded = 0;
			for (std::size_t i = 0; i < N1-1; ++i)
			{
				bool rowAdded = false;

				for (std::size_t k = 0; k < N2-1; ++k)
				{		
					if (needToAdd[i*(N2-1)+k])
					{
						if (rowAdded)
						{
							colToAdd.insert(k);
							continue;
						}
						else
						{
							rowAdded = true;
						}

						std::vector<TParam> locParams(N2);
						std::vector<TValue> locValues(N2);

						for (std::size_t l=0; l<N2; ++l)
						{
							locParams[l] = (_params[ (i+alreadyRowAdded)*N2+l]+_params[((i+alreadyRowAdded)+1)*N2+l]) / 2.0;
							locValues[l] = _object->Evaluate(locParams[l]);
						}					
					
						++alreadyRowAdded;
						std::size_t index = i + alreadyRowAdded;

						_params.insert(_params.begin()+index*N2, locParams.begin(), locParams.end());
						_values.insert(_values.begin()+index*N2, locValues.begin(), locValues.end());

						colToAdd.insert(k);

						break;
					}
				}
			}

			
			_dimensions[0] += alreadyRowAdded;

			int alreadyColAdded = 0;

			for (auto it = colToAdd.begin(); it != colToAdd.end(); ++it)
			{			
				for (std::size_t i = 0; i < N1; ++i)
				{
					TParam locParam = (_params[i*N2 + (*it) + i] + _params[i*N2 + (*it)+1 + i])/2.0;
					TValue locValue = _object->Evaluate(locParam);
					std::size_t index = i*(N2+alreadyColAdded) + (*it) + i+1;
					_params.insert(_params.begin()+index, locParam);
					_values.insert(_values.begin()+index, locValue);
				}
				++alreadyColAdded;
			}

			_dimensions[1] += alreadyColAdded;
		}
		else
		{

		}
	}

	virtual void MakeApproximation() = 0;

    virtual void CacheValues(const std::vector<TParam>& iParams)
    {
        std::vector<std::pair<TParam,TValue>> newCached(iParams.size());

        _pUtils->RunInParallel
            ( 
            [&](int i)
            {
                newCached[i].first = iParams[i];
                newCached[i].second = _object->Evaluate(iParams[i]);
            }, 0, (int)iParams.size()
            );


        _cache.insert(_cache.end(), newCached.begin(), newCached.end());
    }

    const TValue& GetExactValue(const TParam& iParam) const
    {
        auto comp = [iParam](const std::pair<TParam, TValue>& tripleArr) -> bool
        {
            return (iParam == tripleArr.first);
        };

        auto it = std::find_if(_cache.begin(), _cache.end(), comp);

        assert(it != _cache.end());
        
        return (*it).second;
    }

    virtual void CacheParams(const std::vector<TParam>& iParams, std::vector<TValue>& oValues)
    {
        std::vector<TParam> params;
        for (std::size_t i=0; i<iParams.size(); ++i)
        {
            TParam param = iParams[i];

            auto comp = [param](const std::pair<TParam, TValue>& tripleArr) -> bool
            {
                return (param == tripleArr.first);
            };

            auto it = std::find_if(_cache.begin(), _cache.end(), comp);

            if (it == _cache.end())
                params.push_back(param);
        }

        CacheValues(params);

        for (std::size_t i=0;i<iParams.size();++i)
        {
            oValues[i] = GetExactValue(iParams[i]);
        }
    }

	void FillParamsAndValues(int N)
	{
		_object->GetParams(N, _params, _dimensions);

		N = (int)_params.size();
		_values.resize(N);

        _pUtils->RunInParallel(
                                [&](int i)
		                        {
			                        _values[i] = _object->Evaluate(_params[i]);
		                        }, 0, N);
	}

	virtual TValue Evaluate(const TParam& iParam) const override
	{
		return _approxObject->Evaluate(iParam);
	}

	virtual bool CheckValue( const TParam& iParam, double iTolerance, const TValue& oExactValue )
	{
		TValue approxValue = Evaluate(iParam);
		double magn = std::sqrt((oExactValue - approxValue)*(oExactValue - approxValue));
		return magn > iTolerance;
	}

protected:

    std::vector<std::pair<TParam,TValue>> _cache;

	std::vector<std::size_t> _dimensions;

	std::shared_ptr<TValueParamObject<TValue,TParam>> _object;

public:	
    
    std::vector<TParam> _params;
	std::vector<TValue> _values;

	std::shared_ptr<TValueParamObject<TValue,TParam>> _approxObject;

    ParallelUtilsPtr _pUtils;
};