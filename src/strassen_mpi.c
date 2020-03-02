#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "strassen_pre_processamento.c"
#include "mpi.h"

#define ALGORITMO "strassen_mpi"

typedef unsigned long int ulint;

double *strassen_1(ulint n);
double *strassen_2(double *A, double *B, ulint n);
double *strassen_3(double *A, double *B, ulint n);
double *sum(double *A, double *B, ulint n);
double *sum3(double *A, double *B, double *C, ulint n);
double *sub(double *A, double *B, ulint n);
double *split(double *M, double *M11, double *M12, double *M21, double *M22, ulint n);
double *join(double *C11, double *C12, double *C21, double *C22, ulint n);

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
	MPI_Pcontrol(0);

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
	if (rank == 0)
	{
		pre_process(n, path_matriz_A, path_matriz_B);
	}
	MPI_Barrier(MPI_COMM_WORLD);

	//---------------------------------------------------------------------------------
	start = MPI_Wtime();

	double *C = strassen_1(n);

	end = MPI_Wtime();
	//---------------------------------------------------------------------------------

	double exec_time = (end - start) - read_time;
	printLogMPI(log_path, ALGORITMO, n, exec_time, read_time, rank, world_size);

	// SAÍDAS
	if (rank == 0)
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
double *strassen_1(ulint n)
{
	ulint blockSize = n / (ulint)2;
	ulint mallocSize = (ulint)blockSize * (ulint)blockSize * (ulint)sizeof(double);

	double *A11 = read11('A', n);
	double *A12 = read12('A', n);
	double *A21 = read21('A', n);
	double *A22 = read22('A', n);
	double *B11 = read11('B', n);
	double *B12 = read12('B', n);
	double *B21 = read21('B', n);
	double *B22 = read22('B', n);

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

	return join(C11, C12, C21, C22, n);
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
