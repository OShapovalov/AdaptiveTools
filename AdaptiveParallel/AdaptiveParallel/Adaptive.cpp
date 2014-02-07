#include "TPLdefines.h"

#include "TDoubleDoubleObject.h"
#include "OutputToGrapher.h"
#include "DoubleDoubleApproximation.h"
#include "DoubleDoubleFunctions.h"
#include "TValueParamObject.h"
#include "tplForward.h"
#include "SurfaceFunctions.h"
#include "TVect3Vec2Approximation.h"

#include <iostream>
#include "ParallelUtils.h"
#include "Electron.h"

// Utilities and system includes
#include <assert.h>
//#include <helper_string.h>  // helper for shared functions common to CUDA SDK samples

// CUDA runtime
#include <cuda_runtime.h>
#include <cublas_v2.h>
#include "AbstractParallel.h"

// CUDA and CUBLAS functions
//#include <helper_functions.h>

#ifndef min
#define min(a,b) ((a < b) ? a : b)
#endif
#ifndef max
#define max(a,b) ((a > b) ? a : b)
#endif

typedef struct _matrixSize      // Optional Command-line multiplier for matrix sizes
{
    unsigned int uiWA, uiHA, uiWB, uiHB, uiWC, uiHC;
} sMatrixSize;

void inline checkError(cublasStatus_t status, const char *msg)
{
    if (status != CUBLAS_STATUS_SUCCESS)
    {
        printf("%s", msg);
        exit(EXIT_FAILURE);
    }
}

int matrixMultiply(float *h_A, float *h_B, float *h_C, sMatrixSize &matrix_size)
{
    cudaError_t error;

    // use a larger block size for Fermi and above
    int block_size = 32;

    // allocate host memory for matrices A and B
    unsigned int size_A = matrix_size.uiWA * matrix_size.uiHA;
    unsigned int mem_size_A = sizeof(float) * size_A;
    unsigned int size_B = matrix_size.uiWB * matrix_size.uiHB;
    unsigned int mem_size_B = sizeof(float) * size_B;

    // allocate device memory
    float *d_A, *d_B, *d_C;
    unsigned int size_C = matrix_size.uiWC * matrix_size.uiHC;
    unsigned int mem_size_C = sizeof(float) * size_C;

    error = cudaMalloc((void **) &d_A, mem_size_A);

    if (error != cudaSuccess)
    {
        printf("cudaMalloc d_A returned error code %d, line(%d)\n", error, __LINE__);
        exit(EXIT_FAILURE);
    }

    error = cudaMalloc((void **) &d_B, mem_size_B);

    if (error != cudaSuccess)
    {
        printf("cudaMalloc d_B returned error code %d, line(%d)\n", error, __LINE__);
        exit(EXIT_FAILURE);
    }

    // copy host memory to device
    error = cudaMemcpy(d_A, h_A, mem_size_A, cudaMemcpyHostToDevice);

    if (error != cudaSuccess)
    {
        printf("cudaMemcpy d_A h_A returned error code %d, line(%d)\n", error, __LINE__);
        exit(EXIT_FAILURE);
    }

    error = cudaMemcpy(d_B, h_B, mem_size_B, cudaMemcpyHostToDevice);

    if (error != cudaSuccess)
    {
        printf("cudaMemcpy d_B h_B returned error code %d, line(%d)\n", error, __LINE__);
        exit(EXIT_FAILURE);
    }

    error = cudaMalloc((void **) &d_C, mem_size_C);

    if (error != cudaSuccess)
    {
        printf("cudaMalloc d_C returned error code %d, line(%d)\n", error, __LINE__);
        exit(EXIT_FAILURE);
    }

    // setup execution parameters
    dim3 threads(block_size, block_size);
    dim3 grid(matrix_size.uiWC / threads.x, matrix_size.uiHC / threads.y);

    // create and start timer
    //printf("Computing result using CUBLAS...");

    // execute the kernel
    int nIter = 30;

    // CUBLAS version 2.0
    {
        cublasHandle_t handle;

        cublasStatus_t ret;

        ret = cublasCreate(&handle);

        if (ret != CUBLAS_STATUS_SUCCESS)
        {
            printf("cublasCreate returned error code %d, line(%d)\n", ret, __LINE__);
            exit(EXIT_FAILURE);
        }

        const float alpha = 1.0f;
        const float beta  = 0.0f;
        //Perform warmup operation with cublas
        ret = cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, matrix_size.uiWB, matrix_size.uiHA, matrix_size.uiWA, &alpha, d_B, matrix_size.uiWB, d_A, matrix_size.uiWA, &beta, d_C, matrix_size.uiWA);

        if (ret != CUBLAS_STATUS_SUCCESS)
        {
            printf("cublasSgemm returned error code %d, line(%d)\n", ret, __LINE__);
            exit(EXIT_FAILURE);
        }

        // Allocate CUDA events that we'll use for timing
        cudaEvent_t start;
        error = cudaEventCreate(&start);

        if (error != cudaSuccess)
        {
            fprintf(stderr, "Failed to create start event (error code %s)!\n", cudaGetErrorString(error));
            exit(EXIT_FAILURE);
        }

        cudaEvent_t stop;
        error = cudaEventCreate(&stop);

        if (error != cudaSuccess)
        {
            fprintf(stderr, "Failed to create stop event (error code %s)!\n", cudaGetErrorString(error));
            exit(EXIT_FAILURE);
        }

        // Record the start event
        error = cudaEventRecord(start, NULL);

        if (error != cudaSuccess)
        {
            fprintf(stderr, "Failed to record start event (error code %s)!\n", cudaGetErrorString(error));
            exit(EXIT_FAILURE);
        }

        //for (int j = 0; j < nIter; j++)
        {
            //note cublas is column primary!
            //need to transpose the order
            ret = cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, matrix_size.uiWB, matrix_size.uiHA, matrix_size.uiWA, &alpha, d_B, matrix_size.uiWB, d_A, matrix_size.uiWA, &beta, d_C, matrix_size.uiWA);

            if (ret != CUBLAS_STATUS_SUCCESS)
            {
                printf("cublasSgemm returned error code %d, line(%d)\n", ret, __LINE__);
                exit(EXIT_FAILURE);
            }
        }

        //printf("done.\n");

        // Record the stop event
        error = cudaEventRecord(stop, NULL);

        if (error != cudaSuccess)
        {
            fprintf(stderr, "Failed to record stop event (error code %s)!\n", cudaGetErrorString(error));
            exit(EXIT_FAILURE);
        }

        // Wait for the stop event to complete
        error = cudaEventSynchronize(stop);

        if (error != cudaSuccess)
        {
            fprintf(stderr, "Failed to synchronize on the stop event (error code %s)!\n", cudaGetErrorString(error));
            exit(EXIT_FAILURE);
        }

        float msecTotal = 0.0f;
        error = cudaEventElapsedTime(&msecTotal, start, stop);

        if (error != cudaSuccess)
        {
            fprintf(stderr, "Failed to get time elapsed between events (error code %s)!\n", cudaGetErrorString(error));
            exit(EXIT_FAILURE);
        }

        // Compute and print the performance
        //float msecPerMatrixMul = msecTotal / nIter;
        //double flopsPerMatrixMul = 2.0 * (double)matrix_size.uiWA * (double)matrix_size.uiHA * (double)matrix_size.uiWB;
        //double gigaFlops = (flopsPerMatrixMul * 1.0e-9f) / (msecPerMatrixMul / 1000.0f);
        //printf(
        //    "Performance= %.2f GFlop/s, Time= %.3f msec, Size= %.0f Ops\n",
        //    gigaFlops,
        //    msecPerMatrixMul,
        //    flopsPerMatrixMul);

        // copy result from device to host
        error = cudaMemcpy(h_C, d_C, mem_size_C, cudaMemcpyDeviceToHost);

        if (error != cudaSuccess)
        {
            printf("cudaMemcpy h_CUBLAS d_C returned error code %d, line(%d)\n", error, __LINE__);
            exit(EXIT_FAILURE);
        }

        checkError(cublasDestroy(handle), "cublasDestroy() error!\n");
    }

    // clean up memory

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    cudaDeviceReset();
    return 0;
}

//static int nInSettings = 0;

//#include <chrono>

//auto start = std::chrono::high_resolution_clock::now();
// do something time-consuming
//auto end = std::chrono::high_resolution_clock::now();

//double timeTakenInSeconds = (end - start).count() * ((double) std::chrono::high_resolution_clock::period::num / std::chrono_high_resolution_clock::period::den);

 int main(void)
 {	 
  //   float step = 0.000000000003f;
  //   std::vector<TPL_Point> tplPoints;
  //   Electron ele(tplPoints, step);

  //   for (std::size_t i=0;i<100;++i)
  //   {
  //       ele.MakeStep(step);
  //   }

	 ////TDoubleDoubleObjectPtr f2 = std::make_shared<Tfsin>();
	 //auto sinSurf = std::make_shared<TSomeSurface>();

  //   std::vector<ParallelUtils::Technology> techs;

  //   techs.push_back(ParallelUtils::Serial);
  //   //techs.push_back(ParallelUtils::BoostThreads);
  //   //techs.push_back(ParallelUtils::CilkPlus);
  //   auto pUtils1 = std::make_shared<ParallelUtils>(techs);	

	 //auto approx = std::make_shared<TVec3Vec2Approximation>(sinSurf, pUtils1);
	 ////auto approx2 = std::make_shared<TDoubleDoubleApproximation>(f2);

	 ////auto start = AbstractParallel::GetTime();
	 //approx->MakeApprox(10, 1e-2);
	 ////approx2->MakeApprox(10, 1e-5);
	 ////std::cout << "Time: " << (AbstractParallel::GetTime() - start) << std::endl; 

	 ////OutputToGrapher(approx2, "Approx.txt", 1000);
	 ////OutputToGrapher(sinSurf, "Exact.txt", 10000);

  //   return 0;

	 const int N = 512;

	 float* a = new float[N*N];
	 float* b = new float[N*N];
	 float* res = new float[N*N];

     std::fill(a,&a[N*N], 1.0);
     std::fill(b,&b[N*N], 2.0);
     std::fill(res,&res[N*N], 0.0);

     // use a larger block size for Fermi and above
     //int block_size = 32;

     auto labmda = [a,b,res](int start, int end) -> double
     {
        double timeStart = AbstractParallel::GetTime();

         sMatrixSize matrix_size;

         matrix_size.uiWA = 512;
         matrix_size.uiHA = 512;
         matrix_size.uiWB = 512;
         matrix_size.uiHB = 512;
         matrix_size.uiWC = 512;
         matrix_size.uiHC = 512;

         matrixMultiply(a,b,res,matrix_size);

         return AbstractParallel::GetTime() - timeStart;
     };

    std::function<void (int)> f1, someMethod;

    int iterStart = 0;
    int iterEnd = 10;

    std::vector<ParallelUtils::Technology> technologies;

    technologies.push_back(ParallelUtils::Serial);
    //technologies.push_back(ParallelUtils::BoostThreads);
    technologies.push_back(ParallelUtils::PPL);

    ParallelUtils pUtils(technologies);	 
    
    //for(int i=0;i<N;i++)
		    //  for(int j=0;j<N;j++)
		    //  {
			    //   res[i*N+j]=0;
			    //   for (int k=0;k<N;k++) 
				//    res[i*N+j]+=a[i*N+k]*b[k*N+j];
		    //  }

    /*for(int i=0;i<N;i++)*/

    std::vector<std::function<double (int,int)>> addImpls;
    addImpls.push_back(labmda);

    pUtils.RunInParallel([&](int i)
    {
        for(int j=0;j<N;j++)
        {
            res[i*N+j]=0;
            for (int k=0;k<N;k++) 
                res[i*N+j]+=a[i*N+k]*b[k*N+j];
        }
    }
    , 0, N, addImpls);

    //pUtils.RunInParallel([&](int i)
    //{
    //    res[i] = a[i] + b[i];
    //}
    //, 0, N);

	//...
	//pUtils.RunInParallel(f1, iterStart, iterEnd);
	//...

	//...
	//double time = pUtils.GetTimeForFunction(someMethod);
	//...


	//...
	//ParallelUtils::OutputStatisticsToFile();
	//...



	delete[] a;
	delete[] b;
	delete[] res;

	//system ("pause");
 }

//#include <stdio.h>
//#include <omp.h>
//
// typedef unsigned long long INT_TYPE;
//
// INT_TYPE
//     fib_serial( const INT_TYPE n ) {
//         if( n<2 )
//             return n;
//         else
//             return fib_serial(n-1)+fib_serial(n-2);
// }


 //int main( int argc, char *argv[] ) {

 //    INT_TYPE n, answer, expected;
 //    double time1, time2;

 //    n = 41;
 //    expected = 165580141;   // 41
 //    time1 = AbstractParallel::GetTime();

 //    answer = fib_serial( n );

 //    time2 = AbstractParallel::GetTime() - time1;

 //    printf ("\ntime  %lf \n", time2);

 //    if( answer != expected ) {
 //        printf( "failed: answer=%llu, expected=%llu\n", answer, expected );
 //    } else {
 //        printf( "passed\n" );
 //    }

 //    return answer - expected;
 //}


// parallel-fibonacci.cpp
// compile with: /EHsc
//#include <windows.h>
//#include <ppl.h>
//#include <concurrent_vector.h>
//#include <array>
//#include <vector>
//#include <tuple>
//#include <algorithm>
//#include <iostream>

// Computes the nth Fibonacci number.
//int fibonacci(int n)
//{
//    if(n < 2)
//        return n;
//    return fibonacci(n-1) + fibonacci(n-2);
//}

//int wmain()
//{
//    __int64 elapsed;
//
//    // An array of Fibonacci numbers to compute.
//    array<int, 4> a = { 24, 26, 41, 42 };
//
//    // The results of the serial computation.
//    vector<tuple<int,int>> results1;
//
//    // The results of the parallel computation.
//    concurrent_vector<tuple<int,int>> results2;
//
//    // Use the for_each algorithm to compute the results serially.
//    elapsed = time_call([&] 
//    {
//        for_each (begin(a), end(a), [&](int n) {
//            results1.push_back(make_tuple(n, fibonacci(n)));
//        });
//    });   
//    wcout << L"serial time: " << elapsed << L" ms" << endl;
//
//    // Use the parallel_for_each algorithm to perform the same task.
//    elapsed = time_call([&] 
//    {
//        parallel_for_each (begin(a), end(a), [&](int n) {
//            results2.push_back(make_tuple(n, fibonacci(n)));
//        });
//
//        // Because parallel_for_each acts concurrently, the results do not 
//        // have a pre-determined order. Sort the concurrent_vector object
//        // so that the results match the serial version.
//        sort(begin(results2), end(results2));
//    });   
//    wcout << L"parallel time: " << elapsed << L" ms" << endl << endl;
//
//    // Print the results.
//    for_each (begin(results2), end(results2), [](tuple<int,int>& pair) {
//        wcout << L"fib(" << get<0>(pair) << L"): " << get<1>(pair) << endl;
//    });
//}
