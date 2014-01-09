#pragma once

#include <Windows.h>
#include <stdlib.h>
#include "AbstractParallel.h"
#include <set>
#include <vector>
#include "TValueParamObject.h"
#include "Statistics.h"

template< class TValue, class TParam >
class TValueParamApproximation : public TValueParamObject<TValue, TParam>
{
public:

	TValueParamApproximation(std::shared_ptr<TValueParamObject<TValue,TParam>> iObject) : _object(iObject){};

	static std::shared_ptr<TValueParamApproximation<TValue,TParam>> Create(std::shared_ptr<TValueParamObject<TValue,TParam>> iObject)
	{
		return std::make_shared<TValueParamApproximation<TValue,TParam>>(iObject);
	}

	void MakeApprox(double iTolerance)
	{
		return this->MakeApproxOMP(iTolerance, 1);
	}

protected:

	TValueParamApproximation(){};

	virtual void GetIntervals(std::vector<Interval>& oIntervals) const override
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

	virtual void MakeApproxOMP(double iTolerance, int nOMP)
	{
		std::size_t n = 4;
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

			int i, N = (int)params.size();

			std::vector<bool> needToAdd(N);
			std::vector<TValue> exactValues(N);

//#pragma omp parallel for shared(needToAdd, params, exactValues, iTolerance) if (N>nOMP)
			for (i = 0; i < N; ++i )
			{
				needToAdd[i] = GetValue(params[i], iTolerance, exactValues[i]);
			}		

			if (std::find(needToAdd.begin(),needToAdd.end(),true)!=needToAdd.end())
				isFinished = false;

			Insert(params, needToAdd, exactValues);
		}

	}

	void FillParamsAndValues(std::size_t N)
	{
		_object->GetParams(N, _params, _dimensions);

		N = _params.size();
		_values.resize(N);

		for (std::size_t i=0;i<N;++i)
		{
			_values[i] = _object->Evaluate(_params[i]);
		}
	}

	virtual int GetNumberForOMP()
	{
		std::size_t N = 100;
		FillParamsAndValues(N);
		MakeApproximation();

		return AbstractParallel::GetNumberOMP([this]()->bool
		{ 			
			TValue exactValue;
			return GetValue(this->_params[ rand()%this->_params.size()], 0.1, exactValue); 
		});
	}

	virtual TValue Evaluate(const TParam& iParam) const override
	{
		return _approxObject->Evaluate(iParam);
	}

	virtual bool GetValue( const TParam& iParam, double iTolerance, TValue& oExactValue )
	{
		oExactValue = _object->Evaluate(iParam);
		TValue approxValue = Evaluate(iParam);
		double magn = std::sqrt((oExactValue - approxValue)*(oExactValue - approxValue));
		return magn > iTolerance;
	}

protected:

	std::vector<TParam> _params;
	std::vector<TValue> _values;

	std::vector<std::size_t> _dimensions;

	std::shared_ptr<TValueParamObject<TValue,TParam>> _object;

	std::shared_ptr<TValueParamObject<TValue,TParam>> _approxObject;
};