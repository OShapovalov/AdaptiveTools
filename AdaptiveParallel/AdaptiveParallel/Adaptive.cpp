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

// умножение матриц на видеокарте
void matrixMultiply(float *h_A, float *h_B, float *h_C, int N)
{
    unsigned int size_A = N * N;
    unsigned int mem_size_A = sizeof(float) * size_A;
    unsigned int size_B = N * N;
    unsigned int mem_size_B = sizeof(float) * size_B;

    // выдел€ем пам€ть на графическом устройстве
    float *d_A, *d_B, *d_C;
    unsigned int size_C = N * N;
    unsigned int mem_size_C = sizeof(float) * size_C;

    cudaMalloc((void **) &d_A, mem_size_A);
    cudaMalloc((void **) &d_B, mem_size_B);
    cudaMalloc((void **) &d_C, mem_size_C);

    // копируем матрицы A и B в графическую пам€ть
    cudaMemcpy(d_A, h_A, mem_size_A, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, mem_size_B, cudaMemcpyHostToDevice);

    cublasHandle_t handle;
    cublasCreate(&handle);

    // запускаем расчет на графическом ускорителе
    const float alpha = 1.0f;
    const float beta  = 0.0f;
    cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, N, N, N, &alpha, d_B, N, d_A, N, &beta, d_C, N);

    // копируем матрицу результата C из графической пам€ти в оперативную
    cudaMemcpy(h_C, d_C, mem_size_C, cudaMemcpyDeviceToHost);

    // освобождаем пам€ть на графической карте 
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    cudaDeviceReset();
}

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

	 TDoubleDoubleObjectPtr f2 = std::make_shared<Tfsin>();
	 //auto sinSurf = std::make_shared<TSomeSurface>();

     std::vector<ParallelUtils::Technology> techs;

     techs.push_back(ParallelUtils::Serial);
     techs.push_back(ParallelUtils::BoostThreads);
     techs.push_back(ParallelUtils::CilkPlus);
     auto pUtils1 = std::make_shared<ParallelUtils>(techs, "SurfApprox.ini");	

	 //auto approx = std::make_shared<TVec3Vec2Approximation>(sinSurf, pUtils1);
	 auto approx2 = std::make_shared<TDoubleDoubleApproximation>(f2, pUtils1);

	 ////auto start = AbstractParallel::GetTime();
	 //approx->MakeApprox(10, 1e-2);
	 approx2->MakeApprox(10, 1e-3);
	 ////std::cout << "Time: " << (AbstractParallel::GetTime() - start) << std::endl; 

	 ////OutputToGrapher(approx2, "Approx.txt", 1000);
	 ////OutputToGrapher(sinSurf, "Exact.txt", 10000);

     return 0;

	 const int N = 512;

	 float* a = new float[N*N];
	 float* b = new float[N*N];
	 float* res = new float[N*N];

     std::fill(a,&a[N*N], 1.0f);
     std::fill(b,&b[N*N], 2.0f);
     std::fill(res,&res[N*N], 0.0f);

     // создаем обертку (л€мбда-функцию) дл€ умножени€ матриц
     // на графическом ускорителе с замером времени
     auto labmdaMul = [a,b,res](int start, int end) -> double
     {
         double timeStart = AbstractParallel::GetTime();

         matrixMultiply(a,b,res,end);

         return AbstractParallel::GetTime() - timeStart;
     };

    std::function<void (int)> f1, someMethod;

    int iterStart = 0;
    int iterEnd = 10;    
    
    //technologies.push_back(ParallelUtils::BoostThreads);

    // перечисл€ем технологии, которые будут использоватьс€ 
    // дл€ запуска на центральном процессоре
    std::vector<ParallelUtils::Technology> technologies;
    technologies.push_back(ParallelUtils::Serial);
    technologies.push_back(ParallelUtils::PPL);
 
    // перечисл€ем набор реализаций дл€ сопроцессоров
    // в данном случае - умножение матриц на видеокарте
    std::vector<std::pair<std::function<double (int,int)>,bool>> addImpls;
    addImpls.push_back(std::make_pair(labmdaMul,true));
    
    // создаем класс, управл€ющий параллелизмом в проекте
    ParallelUtils pUtils(technologies, "MatrixMul.ini");

    // запускаем умножение матриц с выбранными  параметрами распаралеливани€
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

	delete[] a;
	delete[] b;
	delete[] res;
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
