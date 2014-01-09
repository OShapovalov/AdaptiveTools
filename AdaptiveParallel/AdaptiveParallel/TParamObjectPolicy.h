#pragma once

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