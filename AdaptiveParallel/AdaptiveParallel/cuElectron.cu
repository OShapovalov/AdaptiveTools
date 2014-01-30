#include "cuElectron.h"
#include "Electron.h"

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include "Constants.h"

void /*cuElectron::*/cuMalloc(void **cuElectrons, int memSize)
{
    cudaMalloc(cuElectrons, memSize);
}

extern "C" __global__ void pole_cuda(TPL_Point *cuElectrons, int b1, int e1, float QK, float MK)
{

	int i = blockIdx.x*blockDim.x+threadIdx.x;

	if (i<e1-b1)
	{
		i+=b1;

		float v;
		 cuElectrons[i].ax=-1*QK*cuElectrons[i].vy*B_const/MK;
		 //Y
		 if ( 
             std::abs(cuElectrons[i].z-
                                        Z1) 
             <= 
                    std::abs(cuElectrons[i].z-
                                                Z2)
             )
		 {
			 v=cos(1.57*std::abs(cuElectrons[i].z-Z1)/(Z2-((Z1+Z2)/2))); 
		 }
		 else
		 {
			 v=cos(1.57*std::abs(cuElectrons[i].z-Z2)/(Z2-((Z1+Z2)/2)));
		 }
		 cuElectrons[i].ay=(QK*cuElectrons[i].vx*B_const/MK)-(QK*E_const/MK)+(QK*E_const/MK)*std::abs(v*v*v*v*v*v*v*v*v*v*v*v*v*v*v);
		 //Z
		 if(std::abs(cuElectrons[i].z-Z1)<=std::abs(cuElectrons[i].z-Z2))
		 {
			 v=cos(1.57*std::abs(cuElectrons[i].z-Z1)/(Z2-((Z1+Z2)/2)));
			 cuElectrons[i].az=std::abs((QK*E_const/MK)*v*v*v*v*v*v*v*v*v*v*v*v*v*v*v);
		 }
		 else
		 {
			 v=cos(1.57*std::abs(cuElectrons[i].z-Z2)/(Z2-((Z1+Z2)/2)));
			 cuElectrons[i].az=(-1)*std::abs((QK*E_const/MK)*v*v*v*v*v*v*v*v*v*v*v*v*v*v*v);
		 }
	}
}


extern "C" __global__ void raschet(int *cuElectrons_number, TPL_Point *cuElectrons, int b1,int e1, int b2,int e2, int Count, float MK, float QK, int final)
{
	int i = blockIdx.x*blockDim.x+threadIdx.x;

	i*=32;

	if (i<e1-b1)
	{
		i+=b1;

		float dx,dy,dz,powe;

		if (i<final)
		for(int j=b2;j<e2;++j)
		if (j<Count)
		{
			for (int t=0;t<32;t++)
			  if(((cuElectrons_number[i+t]-Kx)<=cuElectrons_number[j])&&((cuElectrons_number[i+t]+Kx)>=cuElectrons_number[j]))
			    if((i+t!=j)&&(i+t<final))
			{
			 dx=cuElectrons[i+t].x-cuElectrons[j].x;
			 dy=cuElectrons[i+t].y-cuElectrons[j].y;
			 dz=cuElectrons[i+t].z-cuElectrons[j].z;
			 powe=(float)pow((float)(dx*dx+dy*dy+dz*dz),(float)0.5)*num_k/MK*QK*QK/(dx*dx+dy*dy+dz*dz);
			 cuElectrons[i+t].ax+=dx/powe;
			 cuElectrons[i+t].ay+=dy/powe;
			 cuElectrons[i+t].az+=dz/powe;
			 }


		 /*if(((cuElectrons_number[i]-Kx)<=cuElectrons_number[j])&&((cuElectrons_number[i]+Kx)>=cuElectrons_number[j]))
			 if(i!=j)
			 {
			 dx=cuElectrons[i].x-cuElectrons[j].x;
			 dy=cuElectrons[i].y-cuElectrons[j].y;
			 dz=cuElectrons[i].z-cuElectrons[j].z;
			 powe=(float)pow((float)(dx*dx+dy*dy+dz*dz),(float)0.5)*k/MK*QK*QK/(dx*dx+dy*dy+dz*dz);
			 cuElectrons[i].ax+=dx/powe;
			 cuElectrons[i].ay+=dy/powe;
			 cuElectrons[i].az+=dz/powe;
			 }*/



		}
	}
}

extern "C" __global__ void pereschet(int *cuElectrons_number, TPL_Point *cuElectrons, int b1, int e1, float step)
{
	int i = blockIdx.x*blockDim.x+threadIdx.x;

	if (i<e1-b1)
	{
		 i+=b1;

		 cuElectrons[i].x=cuElectrons[i].x+cuElectrons[i].vx*step+(cuElectrons[i].ax*step*step/2);
		 cuElectrons[i].vx+=cuElectrons[i].ax*step;
		 cuElectrons[i].y=cuElectrons[i].y+cuElectrons[i].vy*step+(cuElectrons[i].ay*step*step/2);
		 cuElectrons[i].vy+=cuElectrons[i].ay*step;
		 cuElectrons[i].z=cuElectrons[i].z+cuElectrons[i].vz*step+(cuElectrons[i].az*step*step/2);
		 cuElectrons[i].vz+=cuElectrons[i].az*step;
		 //рассчет места электрона
	}
}

