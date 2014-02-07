#include "Numerical.h"

template <class TGridPtr>
class TNonGrid : public Numerical<TGridPtr>
{
    protected:
        TNonGrid() : Numerical<TGridPtr>()
        {
        };

        TNonGrid(std::shared_ptr<ParallelUtils> pUtils) : Numerical<TGridPtr>(pUtils)
        {
        };

};