#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "strassen_pre_processamento.c"
#include "mpi.h"

#define ALGORITMO "strassen_mpi"
#define MAIN_RANK 1 // O processo que irá juntar os quadrantes de C e imprimir o resutlado

typedef unsigned long int ulint;

double *strassen_1(int world_size, int rank, ulint n);
double *strassen_2(double *A, double *B, ulint n);
double *strassen_3(double *A, double *B, ulint n);
double *sum(double *A, double *B, ulint n);
double *sum3(double *A, double *B, double *C, ulint n);
double *sub(double *A, double *B, ulint n);
double *split(double *M, double *M11, double *M12, double *M21, double *M22, ulint n);
double *join(double *C11, double *C12, double *C21, double *C22, ulint n);
int isMainRank(int world_size, int rank);

int front = 0;

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        printf("Parametros invalidos, verifique...\n");
        return -1;
    }

    ulint n = atoi(argv[1]);
    char *path_matriz_A = argv[2];
    char *path_matriz_B = argv[3];
    const char *log_path = argv[4];
    int output = (argc > 5) ? atoi(argv[5]) : 0;

    ulint rowSize = n * (ulint)sizeof(double);
    ulint matrixSize = n * n;

    // Configurações do MPI
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    MPI_Status status;

    // LOG
    double start, end, r_start, r_end, read_time;
    read_time = 0;

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    pre_process(rank, n, path_matriz_A, path_matriz_B);
    MPI_Barrier(MPI_COMM_WORLD);

    //---------------------------------------------------------------------------------
    start = MPI_Wtime();

    double *C;
    if (rank != 0 || world_size == 1)
    {
        C = strassen_1(world_size, rank, n);
    }

    end = MPI_Wtime();
    //---------------------------------------------------------------------------------

    double exec_time = (end - start) - read_time;
    printLogMPI(log_path, ALGORITMO, n, exec_time, read_time, rank, world_size);

    // SAÍDAS
    if (isMainRank(world_size, rank))
    {
        if (output != 0)
        {
            printMatrix("output/C_strassen_mpi.txt", C, n);
        }

        printf("done\n");
    }

    MPI_Finalize();

    return 0;
}

// Executa a primeira etapa do código, utilizando os quadrantes da primeira divisão da matriz
double *strassen_1(int world_size, int rank, ulint n)
{
    ulint blockSize = n / (ulint)2;
    ulint mallocSize = (ulint)blockSize * (ulint)blockSize * (ulint)sizeof(double);

    double *A11 = read11(rank, 'A', n);
    double *A12 = read12(rank, 'A', n);
    double *A21 = read21(rank, 'A', n);
    double *A22 = read22(rank, 'A', n);
    double *B11 = read11(rank, 'B', n);
    double *B12 = read12(rank, 'B', n);
    double *B21 = read21(rank, 'B', n);
    double *B22 = read22(rank, 'B', n);

    // M1 = (A11 + A22) * (B11 + B22)
    double *M1 = strassen_2(sum(A11, A22, blockSize), sum(B11, B22, blockSize), blockSize);
    // M2 = (A21 + A22) * B11
    double *M2 = strassen_2(sum(A21, A22, blockSize), B11, blockSize);
    // M3 = A11 * (B12 - B22)
    double *M3 = strassen_2(A11, sub(B12, B22, blockSize), blockSize);
    // M4 = A22 * (B21 - B11)
    double *M4 = strassen_2(A22, sub(B21, B11, blockSize), blockSize);
    // M5 = (A11 + A12) * B22
    double *M5 = strassen_2(sum(A11, A12, blockSize), B22, blockSize);
    // M6 = (A21 - A11) * (B11 + B12)
    double *M6 = strassen_2(sub(A21, A11, blockSize), sum(B11, B12, blockSize), blockSize);
    // M7 = (A12 - A22) * (B21 + B22)
    double *M7 = strassen_2(sub(A12, A22, blockSize), sum(B21, B22, blockSize), blockSize);

    // C11 = M1 + M4 - M5 + M7
    double *C11 = sub(sum3(M1, M4, M7, blockSize), M5, blockSize);
    // C12 = M3 + M5
    double *C12 = sum(M3, M5, blockSize);
    // C21 = M2 + M4
    double *C21 = sum(M2, M4, blockSize);
    // C22 = M1 - M2 + M3 + M6
    double *C22 = sub(sum3(M1, M3, M6, blockSize), M2, blockSize);

    printf("M1 %lf %lf %lf %lf\n", M1[0], M1[1], M1[2], M1[3]);
    printf("M4 %lf %lf %lf %lf\n", M4[0], M4[1], M4[2], M4[3]);
    printf("M5 %lf %lf %lf %lf\n", M5[0], M5[1], M5[2], M5[3]);
    printf("M2 %lf %lf %lf %lf\n", M2[0], M2[1], M2[2], M2[3]);
    printf("C21 %lf %lf %lf %lf\n", C21[0], C21[1], C21[2], C21[3]);
    printf("M3 %lf %lf %lf %lf\n", M3[0], M3[1], M3[2], M3[3]);
    printf("C12 %lf %lf %lf %lf\n", C12[0], C12[1], C12[2], C12[3]);
    printf("M6 %lf %lf %lf %lf\n", M6[0], M6[1], M6[2], M6[3]);
    printf("C22 %lf %lf %lf %lf\n", C22[0], C22[1], C22[2], C22[3]);
    printf("M7 %lf %lf %lf %lf\n", M7[0], M7[1], M7[2], M7[3]);
    printf("C11 %lf %lf %lf %lf\n", C11[0], C11[1], C11[2], C11[3]);

    //
    if (isMainRank(world_size, rank))
        return join(C11, C12, C21, C22, n);
    else
        return (double *)calloc(1, sizeof(double));
}

// Executa a segunda etapa do código, utilizado para calcular os primeiros Ms e Cs da etapa 1
double *strassen_2(double *A, double *B, ulint n)
{
    if (n == 1)
    {
        double *C = (double *)calloc(1, sizeof(double));
        C[0] = A[0] * B[0];
        return C;
    }

    ulint blockSize = n / (ulint)2;
    ulint mallocSize = (ulint)blockSize * (ulint)blockSize * (ulint)sizeof(double);

    double *A11 = (double *)malloc(mallocSize);
    double *A12 = (double *)malloc(mallocSize);
    double *A21 = (double *)malloc(mallocSize);
    double *A22 = (double *)malloc(mallocSize);
    split(A, A11, A12, A21, A22, n);
    double *B11 = (double *)malloc(mallocSize);
    double *B12 = (double *)malloc(mallocSize);
    double *B21 = (double *)malloc(mallocSize);
    double *B22 = (double *)malloc(mallocSize);
    split(B, B11, B12, B21, B22, n);

    // M1 = (A11 + A22) * (B11 + B22)
    double *M1 = strassen_3(sum(A11, A22, blockSize), sum(B11, B22, blockSize), blockSize);
    // M2 = (A21 + A22) * B11
    double *M2 = strassen_3(sum(A21, A22, blockSize), B11, blockSize);
    // M3 = A11 * (B12 - B22)
    double *M3 = strassen_3(A11, sub(B12, B22, blockSize), blockSize);
    // M4 = A22 * (B21 - B11)
    double *M4 = strassen_3(A22, sub(B21, B11, blockSize), blockSize);
    // M5 = (A11 + A12) * B22
    double *M5 = strassen_3(sum(A11, A12, blockSize), B22, blockSize);
    // M6 = (A21 - A11) * (B11 + B12)
    double *M6 = strassen_3(sub(A21, A11, blockSize), sum(B11, B12, blockSize), blockSize);
    // M7 = (A12 - A22) * (B21 + B22)
    double *M7 = strassen_3(sub(A12, A22, blockSize), sum(B21, B22, blockSize), blockSize);

    // C11 = M1 + M4 - M5 + M7
    double *C11 = sub(sum3(M1, M4, M7, blockSize), M5, blockSize);
    // C12 = M3 + M5
    double *C12 = sum(M3, M5, blockSize);
    // C21 = M2 + M4
    double *C21 = sum(M2, M4, blockSize);
    // C22 = M1 - M2 + M3 + M6
    double *C22 = sub(sum3(M1, M3, M6, blockSize), M2, blockSize);

    return join(C11, C12, C21, C22, n);
}

double *strassen_3(double *A, double *B, ulint n)
{
    if (n == 1)
    {
        double *C = (double *)calloc(1, sizeof(double));
        C[0] = A[0] * B[0];
        return C;
    }

    ulint blockSize = n / (ulint)2;
    ulint mallocSize = (ulint)blockSize * (ulint)blockSize * (ulint)sizeof(double);

    double *A11 = (double *)malloc(mallocSize);
    double *A12 = (double *)malloc(mallocSize);
    double *A21 = (double *)malloc(mallocSize);
    double *A22 = (double *)malloc(mallocSize);
    split(A, A11, A12, A21, A22, n);
    double *B11 = (double *)malloc(mallocSize);
    double *B12 = (double *)malloc(mallocSize);
    double *B21 = (double *)malloc(mallocSize);
    double *B22 = (double *)malloc(mallocSize);
    split(B, B11, B12, B21, B22, n);

    // M1 = (A11 + A22) * (B11 + B22)
    double *M1 = strassen_3(sum(A11, A22, blockSize), sum(B11, B22, blockSize), blockSize);
    // M2 = (A21 + A22) * B11
    double *M2 = strassen_3(sum(A21, A22, blockSize), B11, blockSize);
    // M3 = A11 * (B12 - B22)
    double *M3 = strassen_3(A11, sub(B12, B22, blockSize), blockSize);
    // M4 = A22 * (B21 - B11)
    double *M4 = strassen_3(A22, sub(B21, B11, blockSize), blockSize);
    // M5 = (A11 + A12) * B22
    double *M5 = strassen_3(sum(A11, A12, blockSize), B22, blockSize);
    // M6 = (A21 - A11) * (B11 + B12)
    double *M6 = strassen_3(sub(A21, A11, blockSize), sum(B11, B12, blockSize), blockSize);
    // M7 = (A12 - A22) * (B21 + B22)
    double *M7 = strassen_3(sub(A12, A22, blockSize), sum(B21, B22, blockSize), blockSize);

    // C11 = M1 + M4 - M5 + M7
    double *C11 = sub(sum3(M1, M4, M7, blockSize), M5, blockSize);
    // C12 = M3 + M5
    double *C12 = sum(M3, M5, blockSize);
    // C21 = M2 + M4
    double *C21 = sum(M2, M4, blockSize);
    // C22 = M1 - M2 + M3 + M6
    double *C22 = sub(sum3(M1, M3, M6, blockSize), M2, blockSize);

    return join(C11, C12, C21, C22, n);
}

double *sum(double *A, double *B, ulint n)
{
    double *C = (double *)calloc(n * n, sizeof(double));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            C[i * n + j] = A[i * n + j] + B[i * n + j];
        }
    }

    return C;
}

double *sum3(double *A, double *B, double *C, ulint n)
{
    double *D = (double *)calloc(n * n, sizeof(double));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            D[i * n + j] = A[i * n + j] + B[i * n + j] + C[i * n + j];
        }
    }

    return D;
}

double *sub(double *A, double *B, ulint n)
{
    double *C = (double *)calloc(n * n, sizeof(double));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            C[i * n + j] = A[i * n + j] - B[i * n + j];
        }
    }

    return C;
}

double *split(double *M, double *M11, double *M12, double *M21, double *M22, ulint n)
{
    ulint blockSize = n / (ulint)2;

    for (int i = 0; i < blockSize; i++)
    {
        for (int j = 0; j < blockSize; j++)
        {
            M11[i * blockSize + j] = M[i * n + j];
            M12[i * blockSize + j] = M[i * n + j + blockSize];
            M21[i * blockSize + j] = M[((i + blockSize) * n) + j];
            M22[i * blockSize + j] = M[((i + blockSize) * n) + j + blockSize];
        }
    }
}

double *join(double *C11, double *C12, double *C21, double *C22, ulint n)
{
    double *C = (double *)calloc(n * n, sizeof(double));
    ulint blockSize = n / (ulint)2;

    for (int i = 0; i < blockSize; i++)
    {
        for (int j = 0; j < blockSize; j++)
        {
            C[i * n + j] = C11[i * blockSize + j];
            C[i * n + j + blockSize] = C12[i * blockSize + j];
            C[((i + blockSize) * n) + j] = C21[i * blockSize + j];
            C[((i + blockSize) * n) + j + blockSize] = C22[i * blockSize + j];
        }
    }

    return C;
}

int isMainRank(int world_size, int rank)
{
    return (rank == MAIN_RANK || world_size == 1);
}