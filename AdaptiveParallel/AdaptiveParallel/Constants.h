#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#define ddd 0
#define num_vx 60000000 //� ����/�������(�������� ���������� �� ��� �)
#define num_y 0.001//� ������(������ ����� �����)
#define num_z 0.01 //� ������(������ ����� �����)
#define Q -1.602e-19 //����� �������

#define E0 8.85e-12 //������������� ����������
#define num_k (1/(4*M_PI*E0)) 
//#define K 0 //����������� ���������� ������
#define num_d 0.005 
#define B_const 0.1 //����. ���. �������� ����
#define E_const 6000000 //������. �������� ����
#define vxv 0.1 //����. ������������� ��������� ������ �� ��� � 
#define I 1 

#define displacementY 0 //�������� ����� ������ �� ��� Y
#define displacementZ 0 //�������� ����� ������ �� ��� Z 
#define width_plastin 0.04 //������ �������
//float Rx (float)(step*time*vx)/(time*count_particle) //������������� ���������� ����� ���������
#define Kx 10 //�������� ��������

//������� ��������� ��������� �������
#define Y1 ((num_y/2)-(num_d/2)-displacementY) 
#define Y2 ((num_y/2)+(num_d/2)-displacementY) 
#define Z1 ((num_z/2)-(width_plastin/2)-displacementZ)
#define Z2 ((num_z/2)+(width_plastin/2)-displacementZ)
//#define level 1 

#define M 9.11e-31 //����� �������