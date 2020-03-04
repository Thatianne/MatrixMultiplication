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

double *strassen_1(ulint n);
double *strassen_2(double *A, double *B, ulint n);
double *strassen_3(double *A, double *B, ulint n);
double *sum(double *A, double *B, ulint n);
double *sum3(double *A, double *B, double *C, ulint n);
double *sub(double *A, double *B, ulint n);
double *split(double *M, double *M11, double *M12, double *M21, double *M22, ulint n);
double *join(double *C11, double *C12, double *C21, double *C22, ulint n);
int isMainRank();
int getMainRank();

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

	double *C = strassen_1(n);

	end = MPI_Wtime();
	//---------------------------------------------------------------------------------

	double exec_time = (end - start) - read_time;
	printLogMPI(log_path, ALGORITMO, n, exec_time, read_time, rank, world_size);
	MPI_Barrier(MPI_COMM_WORLD);

	// SAÍDAS
	if (isMainRank())
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
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	ulint blockSize = n / (ulint)2;
	ulint mallocSize = blockSize * blockSize * (ulint)sizeof(double);

	double *A11 = read11(rank, 'A', n);
	double *A12 = read12(rank, 'A', n);
	double *A21 = read21(rank, 'A', n);
	double *A22 = read22(rank, 'A', n);
	double *B11 = read11(rank, 'B', n);
	double *B12 = read12(rank, 'B', n);
	double *B21 = read21(rank, 'B', n);
	double *B22 = read22(rank, 'B', n);

	double *M1, *M2, *M3, *M4, *M5, *M6, *M7;
	double *C11, *C12, *C21, *C22;
	int c11Init = 0, c12Init = 0, c21Init = 0, c22Init = 0;

	int grupo1 = world_size == 1 || rank == 1;
	int grupo2 = world_size == 1 || rank == 2 || (world_size == 2 && rank == 1);
	int grupo3 = world_size == 1 || rank == 3 || (world_size <= 3 && rank == 1);
	int grupo4 = world_size == 1 || rank == 4 || (world_size <= 2 && rank == 1) || (world_size > 2 && world_size <= 4 && rank == 2);
	int grupo5 = world_size == 1 || rank == 5 || (world_size <= 5 && rank == 1);
	int grupo6 = world_size == 1 || rank == 6 || (world_size <= 3 && rank == 1) || (world_size > 3 && world_size <= 6 && rank == 3);
	int grupo7 = world_size == 1 || rank == 7 || (world_size <= 2 && rank == 1) || (world_size > 2 && world_size <= 7 && rank == 2);
	int rankGrupo1 = (world_size > 1) ? 1 : 0;
	int rankGrupo2 = (world_size > 2) ? 2 : (world_size > 1) ? 1 : 0;
	int rankGrupo3 = (world_size > 3) ? 3 : (world_size > 1) ? 1 : 0;
	int rankGrupo4 = (world_size > 4) ? 4 : (world_size > 2) ? 2 : (world_size > 1) ? 1 : 0;
	int rankGrupo5 = (world_size > 5) ? 5 : (world_size > 1) ? 1 : 0;
	int rankGrupo6 = (world_size > 6) ? 6 : (world_size > 3) ? 3 : (world_size > 1) ? 1 : 0;
	int rankGrupo7 = (world_size > 7) ? 7 : (world_size > 2) ? 2 : (world_size > 1) ? 1 : 0;

	//printf("-----> LOG: %d: %d %d %d %d %d %d %d\n", rank, grupo1, grupo2, grupo3, grupo4, grupo5, grupo6, grupo7);

	MPI_Request reqM1, reqM2, reqM3, reqM4, reqM5, reqM6, reqM7, reqC11, reqC12, reqC21, reqC22;
	MPI_Status status1, status2, status3, status4, status5, status6, status7, statusC11, statusC12, statusC21, statusC22;

	// **************** Task 1 ****************
	if (grupo1)
	{
		// M1 = (A11 + A22) * (B11 + B22)
		M1 = strassen_2(sum(A11, A22, blockSize), sum(B11, B22, blockSize), blockSize);
		MPI_Isend(M1, n, MPI_DOUBLE, rankGrupo6, 1, MPI_COMM_WORLD, &reqM1);
		MPI_Isend(M1, n, MPI_DOUBLE, rankGrupo7, 1, MPI_COMM_WORLD, &reqM1);
	}

	// **************** Task 4 ****************
	if (grupo4)
	{
		// M4 = A22 * (B21 - B11)
		M4 = strassen_2(A22, sub(B21, B11, blockSize), blockSize);
		MPI_Isend(M4, n, MPI_DOUBLE, rankGrupo2, 4, MPI_COMM_WORLD, &reqM4);
		MPI_Isend(M4, n, MPI_DOUBLE, rankGrupo7, 4, MPI_COMM_WORLD, &reqM4);
	}

	// **************** Task 5 ****************
	if (grupo5)
	{
		// M5 = (A11 + A12) * B22
		M5 = strassen_2(sum(A11, A12, blockSize), B22, blockSize);
		MPI_Isend(M5, n, MPI_DOUBLE, rankGrupo3, 5, MPI_COMM_WORLD, &reqM5);
		MPI_Isend(M5, n, MPI_DOUBLE, rankGrupo7, 5, MPI_COMM_WORLD, &reqM5);
	}

	// **************** Task 2 ****************
	if (grupo2)
	{
		M4 = (double *)malloc(mallocSize);
		MPI_Irecv(M4, n, MPI_DOUBLE, rankGrupo4, 4, MPI_COMM_WORLD, &reqM4);

		// M2 = (A21 + A22) * B11
		M2 = strassen_2(sum(A21, A22, blockSize), B11, blockSize);
		MPI_Isend(M2, n, MPI_DOUBLE, rankGrupo6, 2, MPI_COMM_WORLD, &reqM2);

		MPI_Wait(&reqM4, &status4);

		// C21 = M2 + M4
		C21 = sum(M2, M4, blockSize);
		c21Init = 1;
		if (!isMainRank())
			MPI_Isend(C21, n, MPI_DOUBLE, getMainRank(), 10, MPI_COMM_WORLD, &reqC21);
	}

	// **************** Task 3 ****************
	if (grupo3)
	{
		M5 = (double *)malloc(mallocSize);
		MPI_Irecv(M5, n, MPI_DOUBLE, rankGrupo5, 5, MPI_COMM_WORLD, &reqM5);

		// M3 = A11 * (B12 - B22)
		M3 = strassen_2(A11, sub(B12, B22, blockSize), blockSize);
		MPI_Isend(M3, n, MPI_DOUBLE, rankGrupo6, 3, MPI_COMM_WORLD, &reqM3);

		MPI_Wait(&reqM5, &status5);

		// C12 = M3 + M5
		C12 = sum(M3, M5, blockSize);
		c12Init = 1;
		if (!isMainRank())
			MPI_Isend(C12, n, MPI_DOUBLE, getMainRank(), 9, MPI_COMM_WORLD, &reqC12);
	}

	// **************** Task 6 ****************
	if (grupo6)
	{
		M1 = (double *)malloc(mallocSize);
		MPI_Irecv(M1, n, MPI_DOUBLE, rankGrupo1, 1, MPI_COMM_WORLD, &reqM1);

		M2 = (double *)malloc(mallocSize);
		MPI_Irecv(M2, n, MPI_DOUBLE, rankGrupo2, 2, MPI_COMM_WORLD, &reqM2);

		M3 = (double *)malloc(mallocSize);
		MPI_Irecv(M3, n, MPI_DOUBLE, rankGrupo3, 3, MPI_COMM_WORLD, &reqM3);

		// M6 = (A21 - A11) * (B11 + B12)
		M6 = strassen_2(sub(A21, A11, blockSize), sum(B11, B12, blockSize), blockSize);

		MPI_Wait(&reqM1, &status1);
		MPI_Wait(&reqM2, &status2);
		MPI_Wait(&reqM3, &status3);

		// C22 = M1 - M2 + M3 + M6
		C22 = sub(sum3(M1, M3, M6, blockSize), M2, blockSize);
		c22Init = 1;
		if (!isMainRank())
			MPI_Isend(C22, n, MPI_DOUBLE, getMainRank(), 11, MPI_COMM_WORLD, &reqC22);
	}

	// **************** Task 7 ****************
	if (grupo7)
	{
		M1 = (double *)malloc(mallocSize);
		MPI_Irecv(M1, n, MPI_DOUBLE, rankGrupo1, 1, MPI_COMM_WORLD, &reqM1);

		M4 = (double *)malloc(mallocSize);
		MPI_Irecv(M4, n, MPI_DOUBLE, rankGrupo4, 4, MPI_COMM_WORLD, &reqM4);

		M5 = (double *)malloc(mallocSize);
		MPI_Irecv(M5, n, MPI_DOUBLE, rankGrupo5, 5, MPI_COMM_WORLD, &reqM5);

		// M7 = (A12 - A22) * (B21 + B22)
		M7 = strassen_2(sub(A12, A22, blockSize), sum(B21, B22, blockSize), blockSize);

		MPI_Wait(&reqM1, &status1);
		MPI_Wait(&reqM4, &status4);
		MPI_Wait(&reqM5, &status5);

		// C11 = M1 + M4 - M5 + M7
		C11 = sub(sum3(M1, M4, M7, blockSize), M5, blockSize);
		c11Init = 1;
		if (!isMainRank())
			MPI_Isend(C11, n, MPI_DOUBLE, getMainRank(), 8, MPI_COMM_WORLD, &reqC11);
	}

	if (!isMainRank())
		return (double *)calloc(1, sizeof(double));
	else
	{
		// Verificando a inicialização dos quadrantes, pois o main rank pode ter iniciado algum
		if (!c11Init)
		{
			C11 = (double *)malloc(mallocSize);
			MPI_Irecv(C11, n, MPI_DOUBLE, rankGrupo7, 8, MPI_COMM_WORLD, &reqC11);
		}
		if (!c12Init)
		{
			C12 = (double *)malloc(mallocSize);
			MPI_Irecv(C12, n, MPI_DOUBLE, rankGrupo3, 9, MPI_COMM_WORLD, &reqC12);
		}
		if (!c21Init)
		{
			C21 = (double *)malloc(mallocSize);
			MPI_Irecv(C21, n, MPI_DOUBLE, rankGrupo2, 10, MPI_COMM_WORLD, &reqC21);
		}
		if (!c22Init)
		{
			C22 = (double *)malloc(mallocSize);
			MPI_Irecv(C22, n, MPI_DOUBLE, rankGrupo6, 11, MPI_COMM_WORLD, &reqC22);
		}

		if (!c11Init)
			MPI_Wait(&reqC11, &statusC11);
		if (!c12Init)
			MPI_Wait(&reqC12, &statusC12);
		if (!c21Init)
			MPI_Wait(&reqC21, &statusC21);
		if (!c22Init)
			MPI_Wait(&reqC22, &statusC22);

		return join(C11, C12, C21, C22, n);
	}
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

int isMainRank()
{
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	return (world_size > MAIN_RANK && rank == MAIN_RANK) || (world_size < MAIN_RANK && rank == world_size - 1);
}

int getMainRank()
{
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	return (world_size > MAIN_RANK) ? MAIN_RANK : (world_size - 1);
}