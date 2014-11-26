#include "PTLdefines.h"
#include "PTLForward.h"

#include <iostream>
#include "ParallelUtils.h"
#include "ParallelUtilsBase.h"

void strassen_mult(
    int n, double *A, double *B, double *C)
{
    // выделение памяти 
    double *buf = (double *)malloc(13 * n/2 * n/2 * sizeof(double));
    double *s1 = buf + n/2 * n/2 * 0;
    double *s2 = buf + n/2 * n/2 * 1;
    double *s3 = buf + n/2 * n/2 * 2;
    double *s4 = buf + n/2 * n/2 * 3;
    double *s5 = buf + n/2 * n/2 * 4;
    double *s6 = buf + n/2 * n/2 * 5;
    double *s7 = buf + n/2 * n/2 * 6;
    double *s8 = buf + n/2 * n/2 * 7;
    double *s9 = buf + n/2 * n/2 * 8;
    double *s10 = buf + n/2 * n/2 * 9;

    double *t1 = buf + n/2 * n/2 * 10;
    double *t2 = buf + n/2 * n/2 * 11;
    double *t3 = buf + n/2 * n/2 * 12;

    double *sub_a[2][2];
    double *sub_b[2][2];
    double *sub_c[2][2];
    int i, j;
    for (i = 0; i < 2; ++i)
    {
        for (j = 0; j < 2; ++j)
        {
            sub_a[i][j] = &A[(i * 2 + j) * n/2 * n/2];
            sub_b[i][j] = &B[(i * 2 + j) * n/2 * n/2];
            sub_c[i][j] = &C[(i * 2 + j) * n/2 * n/2];
        }
    }

    // операции сложения матриц
#pragma omp task
    matrix_sum(n/2, sub_a[0][0], sub_a[1][1], s1);
#pragma omp task
    matrix_sum(n/2, sub_b[0][0], sub_b[1][1], s2);
#pragma omp task
    matrix_sum(n/2, sub_a[1][0], sub_a[1][1], s3);
#pragma omp task
    matrix_sub(n/2, sub_b[0][1], sub_b[1][1], s4);
#pragma omp task
    matrix_sub(n/2, sub_b[1][0], sub_b[0][0], s5);
#pragma omp task
    matrix_sum(n/2, sub_a[0][0], sub_a[0][1], s6);
#pragma omp task
    matrix_sub(n/2, sub_a[1][0], sub_a[0][0], s7);
#pragma omp task
    matrix_sum(n/2, sub_b[0][0], sub_b[0][1], s8);
#pragma omp task
    matrix_sub(n/2, sub_a[0][1], sub_a[1][1], s9);
#pragma omp task
    matrix_sum(n/2, sub_b[1][0], sub_b[1][1], s10);
#pragma omp taskwait


    // перечисляем технологии для центрального процессора
    std::vector<Technology> technologies;
    technologies.push_back(Technology::Serial);
    technologies.push_back(Technology::TBB);
    technologies.push_back(Technology::OpenMP);

    // создаем класс, управляющий параллелизмом в проекте
    ParallelUtilsBase baseParallel; 
    // добавляем обработку нового объекта распараллеливания
    auto pUtils = baseParallel.AddNewParUtils(technologies, "StrassenMult.xml");
    
    // выделяем память под дополнительные матрицы
    ...
        pUtils->AddSpawnFuction( [&]() {
            strassen_mult(n/2, s3, b[0][0], c[1][0], pUtils);
    } );

    void strassen_mult( int n, double *A, double *B, double *C, ParallelUtilsPtr pUtils)
    {
    // выполняем сложения подматриц

    pUtils->AddSpawnFuction( [&]() {
        strassen_mult(n/2, s1, s2, t1, pUtils);
        } );
    ...
    pUtils->AddSpawnFuction( [&]() {
        strassen_mult(n/2, s9, s10, t3, pUtils);
        } );

    pUtils->RunSpawnFuctions( 3.0 );

    // выполняем сложения подматриц в итоговую матрицу
    }

    //pUtils->AddSpawnFuction( [&]() {
    //    strassen_mult(n/2, s3, sub_b[0][0], sub_c[1][0]);
    //} );

    //pUtils->AddSpawnFuction( [&]() {
    //    strassen_mult(n/2, sub_a[0][0], s4, sub_c[0][1]);
    //} );

    //pUtils->AddSpawnFuction( [&]() {
    //    strassen_mult(n/2, sub_a[1][1], s5, sub_c[0][0]);
    //} );

    //pUtils->AddSpawnFuction( [&]() {
    //    strassen_mult(n/2, s6, sub_b[1][1], t2);
    //} );

    //pUtils->AddSpawnFuction( [&]() {
    //    strassen_mult(n/2, s7, s8, sub_c[1][1]);
    //} );


    // c11 = -p2 + p6
    matrix_sub(n/2, sub_c[1][1], sub_c[1][0], sub_c[1][1]);
    // c11 = p3 - p2 + p6
#pragma omp task
    matrix_sum(n/2, sub_c[1][1], sub_c[0][1], sub_c[1][1]);
    // c10 = p2 + p4
#pragma omp task
    matrix_sum(n/2, sub_c[1][0], sub_c[0][0], sub_c[1][0]);
#pragma omp taskwait
    // c01 = p3 + p5
#pragma omp task
    matrix_sum(n/2, sub_c[0][1], t2, sub_c[0][1]);
    // c00 = p4 - p5
#pragma omp task
    matrix_sub(n/2, sub_c[0][0], t2, sub_c[0][0]);
#pragma omp taskwait
    // c00 = p4 - p5 + p7
    matrix_sum(n/2, sub_c[0][0], t3, sub_c[0][0]);
    // c00 = p1 + p4 - p5 + p7
#pragma omp task
    matrix_sum(n/2, sub_c[0][0], t1, sub_c[0][0]);
    // c11 = p1 + p3 - p2 + p6
#pragma omp task
    matrix_sum(n/2, sub_c[1][1], t1, sub_c[1][1]);
#pragma omp taskwait

    free(buf);
}