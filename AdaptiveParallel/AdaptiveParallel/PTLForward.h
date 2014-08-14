#pragma once

#include <memory>
#include <functional>

typedef std::function<void (int)> IAloneFunction;
typedef std::function<double (int, int)> IManyFunction;
typedef std::function<void (void)> IVoidFunction;

class TDoubleDoubleApproximation;
typedef std::shared_ptr<TDoubleDoubleApproximation> ApproximationPtr;

class TDoubleDoubleObject;
typedef std::shared_ptr<TDoubleDoubleObject> TDoubleDoubleObjectPtr;

template< class TValue >
class TValueDoubleObject;

template< class TValue, class TParam >
class TValueParamObject;

template <typename TValue>
struct TValueObjectPtr
{
	typedef std::shared_ptr< TValueDoubleObject<TValue> > type;
};

template <typename TValue, typename TParam>
struct TValueParamObjectPtr
{
	typedef std::shared_ptr< TValueParamObject<TValue, TParam> > type;
};

template<class TValue, class TParam>
class TValueParamInsertion;

template <typename TValue, typename TParam>
struct TValueParamInsertionPtr
{
	typedef std::shared_ptr< TValueParamInsertion<TValue, TParam> > type;
};

class ParallelTechnology;
typedef std::shared_ptr<ParallelTechnology> ParallelTechnologyPtr;

//class ParallelInfo;
//typedef std::shared_ptr<ParallelInfo> ParallelInfoPtr;

class ParallelTimes;
typedef std::shared_ptr<ParallelTimes> ParallelTimesPtr;

class ParallelUtils;
typedef std::shared_ptr<ParallelUtils> ParallelUtilsPtr;