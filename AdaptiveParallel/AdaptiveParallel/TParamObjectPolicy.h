#pragma once

template<class TParam>
class TParamObjectPolicy
{
public:

    virtual void GetCheckParams(const std::vector<TParam>& iParams, const std::vector<std::size_t>& iDimensions, std::vector<TParam>& oParams) = 0;

    virtual void FillParams( const std::vector<std::vector<double>>& allParams, std::vector<TParam>& oParams);

    //virtual void FillParams( int rec, const std::vector<std::vector<double>>& allParams, std::vector<TParam>& oParams, std::vector<double>& params) = 0
    //{
    //    for (std::size_t i=0; i<allParams[rec].size(); ++i )
    //    {			
    //        if (rec == allParams.size()-1)
    //        {
    //            params.push_back(allParams[rec][i]);
    //            oParams.push_back( TValue(params) );
    //            params.pop_back();
    //        }
    //        else
    //        {
    //            params.push_back(allParams[rec][i]);
    //            FillParams(rec+1, allParams, oParams, params);
    //            params.pop_back();
    //        }
    //    }
    //}
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

    void FillParams(const std::vector<std::vector<double>>& allParams, std::vector<double>& oParams ) 
    {			
        oParams.resize(allParams[0].size());
        for (std::size_t i =0; i<allParams[0].size(); ++i)
        {
            oParams[i] = allParams[0][i];
        }
    }
};

template<>
class TParamObjectPolicy<PTLVector2D>
{
public:

    virtual void GetCheckParams(const std::vector<PTLVector2D>& iParams, const std::vector<std::size_t>& iDimensions, std::vector<PTLVector2D>& oParams)
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

    void FillParams(const std::vector<std::vector<double>>& allParams, std::vector<PTLVector2D>& oParams ) 
    {		
        std::size_t N1 = allParams[0].size();
        std::size_t N2 = allParams[1].size();
        oParams.resize(N1*N2);
        for (std::size_t i =0; i<N1; ++i)
            for (std::size_t k =0; k<N2; ++k)
            {
                oParams[i*N2+k] = PTLVector2D(allParams[0][i],allParams[1][k]);
            }
    }

};

template<>
class TParamObjectPolicy<ptlVector3D>
{
public:

    void FillParams(const std::vector<std::vector<double>>& allParams, std::vector<ptlVector3D>& oParams ) 
    {		
        std::size_t N1 = allParams[0].size();
        std::size_t N2 = allParams[1].size();
        std::size_t N3 = allParams[2].size();
        oParams.resize(N1*N2*N3);
        for (std::size_t i =0; i<N1; ++i)
            for (std::size_t k =0; k<N2; ++k)
                for (std::size_t l =0; l<N3; ++l)
                {
                    oParams[i*N2*N3+k*N3+l] = ptlVector3D(allParams[0][i],allParams[1][k],allParams[2][l]);
                }
    }
};