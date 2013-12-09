#pragma once

#include <memory>
#include <functional>

class TDoubleDoubleApproximation;
typedef std::shared_ptr<TDoubleDoubleApproximation> ApproximationPtr;

class TObject;
typedef std::shared_ptr<TObject> TObjectPtr;

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