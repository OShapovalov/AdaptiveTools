#pragma once
#include "Gravity.h"
#define _USE_MATH_DEFINES
#include <math.h>

double ddd=0;
double vx=60000000;//� ����/�������(�������� ���������� �� ��� �)
double y=0.001;//� ������(������ ����� �����)
double z=0.01;//� ������(������ ����� �����)
double Q=-1.602e-19;//����� �������

double E0=8.85e-12;//������������� ����������
double k=1/(4*M_PI*E0);
double K=0;//����������� ���������� ������
double d=0.005;
double B_const=0.1;//����. ���. �������� ����
double E_const=6000000;//������. �������� ����
double vxv=0.1;//����. ������������� ��������� ������ �� ��� � 
double I=1;

double displacementY=0;//�������� ����� ������ �� ��� Y
double displacementZ=0;//�������� ����� ������ �� ��� Z 
double width_plastin=0.04;//������ �������
//float Rx=(float)(step*time*vx)/(time*count_particle);//������������� ���������� ����� ���������
int Kx=10;//�������� ��������

//������� ��������� ��������� �������
double Y1=(y/2)-(d/2)-displacementY;
double Y2=(y/2)+(d/2)-displacementY;
double Z1=(z/2)-(width_plastin/2)-displacementZ;
double Z2=(z/2)+(width_plastin/2)-displacementZ;
double level=1;

double M=9.11e-31;//����� �������

struct TPL_Point
{
    std::array<double,3> _coords;
    std::array<double,3> _velo;
    std::array<double,3> _accel;
};

class Electron : public Gravity<TPL_Point>
{
public:
    Electron(const std::vector<TPL_Point>& iPoints, double step):Gravity<TPL_Point>(iPoints)
    {
        K=std::abs(I*step/(Q*iPoints.size()));
        QK=Q*K;//����� ������� �����������
        MK=M*K;//����� ������� �����������
    };

    void pole(TPL_Point& iPoint)
    {
        double v;

        iPoint._accel[0] = -QK*iPoint._velo[1]*B_const/MK;
        //Y
        if(std::abs(iPoint._coords[2]-Z1)<=std::abs(iPoint._coords[2]-Z2))
        {
            v=cos(M_PI_2*std::abs(iPoint._coords[2]-Z1)/(Z2-((Z1+Z2)/2))); 
        }
        else
        {
            v=cos(M_PI_2*std::abs(iPoint._coords[2]-Z2)/(Z2-((Z1+Z2)/2)));
        }
        iPoint._accel[1] = (QK*iPoint._velo[0]*B_const/MK)-(QK*E_const/MK)+(QK*E_const/MK)*std::abs(std::pow(v, 15));
        //Z
        if(std::abs(iPoint._coords[2]-Z1)<=std::abs(iPoint._coords[2]-Z2))
        {
            v=cos(M_PI_2*std::abs(iPoint._coords[2]-Z1)/(Z2-((Z1+Z2)/2)));
            iPoint._accel[2] = std::abs((QK*E_const/MK)*std::pow(v, 15));
        }
        else
        {
            v=cos(M_PI_2*std::abs(iPoint._coords[2]-Z2)/(Z2-((Z1+Z2)/2)));
            iPoint._accel[2] = -std::abs((QK*E_const/MK)*std::pow(v, 15));
        }
    }

    virtual void FindAcceleration(TPL_Point& iPoint, double step) override
    {
        pole(iPoint);
    }

    virtual void GetAccel(TPL_Point& iPoint1, TPL_Point& iPoint2) override
    {
        std::array<double, 3> delta = { 
                                        iPoint1._coords[0] - iPoint2._coords[0],
                                        iPoint1._coords[1] - iPoint2._coords[1],
                                        iPoint1._coords[2] - iPoint2._coords[2] 
                                      };
        double r2 = std::pow( delta[0], 2 ) +
                    std::pow( delta[1], 2 ) +
                    std::pow( delta[2], 2 );

        double r = sqrt(r2);

        double F = k*Q*Q / r2; 

        for (int i=0;i<3;++i)
        {
            iPoint1._accel[i] += F * delta[i] / r;
            iPoint2._accel[i] +=  - F * delta[i] / r;
        }         
    }

    virtual void Transform(TPL_Point& iPoint, double step) override
    {
        for (int i=0;i<3;++i)
        {
            iPoint._coords[i] += iPoint._velo[i] * step + iPoint._accel[i] * 0.5 * step * step;
            iPoint._velo[i] += iPoint._accel[i] * step;
            iPoint._accel[i] = 0.0;
        }    
    }

private:
    Electron(){};

    double K;
    double QK;//����� ������� �����������
    double MK;//����� ������� �����������
};