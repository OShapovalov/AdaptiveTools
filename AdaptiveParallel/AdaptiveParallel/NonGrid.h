#include "Numerical.h"

template <class TGridPtr>
class TNonGrid : public Numerical<TGridPtr>
{
    protected:
        //TNonGrid() : Numerical<TGridPtr>()
        //{
        //};

        TNonGrid(ParallelUtilsPtr pUtils) : Numerical<TGridPtr>(pUtils)
        {
        };

};