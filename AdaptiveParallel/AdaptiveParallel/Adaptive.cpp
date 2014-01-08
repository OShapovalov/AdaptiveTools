#include "TDoubleDoubleObject.h"
#include "OutputToGrapher.h"
#include "DoubleDoubleApproximation.h"
#include "DoubleDoubleFunctions.h"
#include "TValueParamObject.h"
#include "forward.h"
#include "SurfaceFunctions.h"
#include "TVect3Vec2Approximation.h"

#include <iostream>
#include "ParallelUtils.h"

static int nInSettings = 0;

//#include <chrono>

//auto start = std::chrono::high_resolution_clock::now();
// do something time-consuming
//auto end = std::chrono::high_resolution_clock::now();

//double timeTakenInSeconds = (end - start).count() * ((double) std::chrono::high_resolution_clock::period::num / std::chrono_high_resolution_clock::period::den);

 int main(void)
 {	 
	 //TObjectPtr f2 = std::make_shared<Tfsin>();
	 //auto sinSurf = std::make_shared<TSomeSurface>();
	 //auto approx = TVec3Vec2Approximation::Create(sinSurf);
	 //auto approx2 = TDoubleDoubleApproximation::Create(f2);

	 //auto start = AbstractParallel::GetTime();
	 //approx->MakeApprox(1e-2);
	 //approx2->MakeApprox(1e-5);
	 //std::cout << "Time: " << (AbstractParallel::GetTime() - start) << std::endl; 

	 //OutputToGrapher(approx2, "Approx.txt", 1000);
	 //OutputToGrapher(sinSurf, "Exact.txt", 10000);

	 const int N = 500;

	 double* a = new double[N*N];
	 double* b = new double[N*N];
	 double* res = new double[N*N];

     std::fill(a,&a[N*N], 1.0);
     std::fill(b,&b[N*N], 2.0);
     std::fill(res,&res[N*N], 0.0);

    std::function<void (int)> f1, someMethod;

    int iterStart = 0;
    int iterEnd = 10;

    ParallelUtils pUtils;	 
    
    //for(int i=0;i<N;i++)
		    //  for(int j=0;j<N;j++)
		    //  {
			    //   res[i*N+j]=0;
			    //   for (int k=0;k<N;k++) 
				//    res[i*N+j]+=a[i*N+k]*b[k*N+j];
		    //  }

    /*for(int i=0;i<N;i++)*/

    pUtils.RunInParallel([&](int i)
    {
        for(int j=0;j<N;j++)
        {
            res[i*N+j]=0;
            for (int k=0;k<N;k++) 
                res[i*N+j]+=a[i*N+k]*b[k*N+j];
        }
    }
    , 0, N);

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
