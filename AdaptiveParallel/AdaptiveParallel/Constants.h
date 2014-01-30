#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#define ddd 0
#define num_vx 60000000 //в метр/секунда(скорость электронов по оси х)
#define num_y 0.001//в метрах(высота рамки входа)
#define num_z 0.01 //в метрах(длинна рамки входа)
#define Q -1.602e-19 //заряд частицы

#define E0 8.85e-12 //электрическая постоянная
#define num_k (1/(4*M_PI*E0)) 
//#define K 0 //коэффициент укрупнения частиц
#define num_d 0.005 
#define B_const 0.1 //магн. инд. внешнего поля
#define E_const 6000000 //непряж. внешнего поля
#define vxv 0.1 //коэф. вариативности скоростей частиц по оси Х 
#define I 1 

#define displacementY 0 //смещение влета потока по оси Y
#define displacementZ 0 //смещение влета потока по оси Z 
#define width_plastin 0.04 //ширина пластин
//float Rx (float)(step*time*vx)/(time*count_particle) //относительное расстояние между частицами
#define Kx 10 //параметр точности

//рассчет координат граничных пластин
#define Y1 ((num_y/2)-(num_d/2)-displacementY) 
#define Y2 ((num_y/2)+(num_d/2)-displacementY) 
#define Z1 ((num_z/2)-(width_plastin/2)-displacementZ)
#define Z2 ((num_z/2)+(width_plastin/2)-displacementZ)
//#define level 1 

#define M 9.11e-31 //масса частицы