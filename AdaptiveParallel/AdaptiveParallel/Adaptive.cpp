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
 //    time1 = omp_get_wtime();

 //    answer = fib_serial( n );

 //    time2 = omp_get_wtime() - time1;

 //    printf ("\ntime  %lf \n", time2);

 //    if( answer != expected ) {
 //        printf( "failed: answer=%llu, expected=%llu\n", answer, expected );
 //    } else {
 //        printf( "passed\n" );
 //    }

 //    return answer - expected;
 //}


#include <stdio.h>
#include <omp.h>

#define SPLITTER 16
 typedef unsigned long long INT_TYPE;

 INT_TYPE
     fib_serial( const INT_TYPE n ) {
         if( n<2 )
             return n;
         else
             return fib_serial(n-1)+fib_serial(n-2);
 }

 INT_TYPE
     fib( INT_TYPE n ) {
         INT_TYPE i, j;
         if( n < SPLITTER ) {
             return fib_serial( n );
         } else {
#pragma omp task shared( i )
             i = fib( n - 1 );
#pragma omp task shared( j )
             j = fib( n - 2 );
#pragma omp taskwait
             return i + j;
         }
 }

 INT_TYPE
     fib_iter( INT_TYPE n ) {
         INT_TYPE f0, f1, f2, i;
         if( n < 2 )
             return n;
         f0 = 0;
         f1 = 1;
         f2 = 1;
         for( i = 2; i <= n; ++i ) {
             f2 = f0 + f1;
             f0 = f1;
             f1 = f2;
         }
         return f2;
 }

 int main( int argc, char *argv[] ) {

     INT_TYPE n, answer, expected;
     double time1, time2;

     n = 41;
     expected =   165580141;   // 41
     //expected = fib_iter( n );
     time1 = omp_get_wtime();
#pragma omp parallel
     {
#pragma omp single
#pragma omp task
         answer = fib( n );
     }
     time2 = omp_get_wtime() - time1;
     //printf( "fib(%d) = %d\n", n, answer );
     printf ("\ntime  %lf \n", time2);

     if( answer != expected ) {
         printf( "failed: answer=%llu, expected=%llu\n", answer, expected );
     } else {
         printf( "passed\n" );
     }

     return answer - expected;
 }
