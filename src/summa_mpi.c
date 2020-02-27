#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "util.c"

#include <time.h>
#include <sys/time.h>

#define ALGORITMO "summa_mpi"

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

	FILE *fpA = fopen(path_matriz_A, "rb");
	FILE *fpB = fopen(path_matriz_B, "rb");
	size_t readed;

	ulint rowSize = n * (ulint)sizeof(double);
	ulint matrixSize = n * n;
	double *A = (double *)malloc(rowSize);
	double a;
	double *B = (double *)malloc(rowSize);
	double *C = (double *)calloc(matrixSize, sizeof(double));
	//---------------------------------------------------------------------------------

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
	//---------------------------------------------------------------------------------

	for (int k = rank; k < n; k += (world_size))
	{
		//================================ LEITURA ================================
		// Lê a coluna 'k' da matriz do arquivo 'fpA' e armazena em A
		fseek(fpA, 0, SEEK_SET);
		fseek(fpA, ((ulint)k * n) * (ulint)sizeof(double), SEEK_SET);
		readed = fread(A, sizeof(double), n, fpA);

		// Lê a linha 'k' da matriz do arquivo 'fpB' e armazena em B
		fseek(fpB, 0, SEEK_SET);
		fseek(fpB, ((ulint)k * n) * (ulint)sizeof(double), SEEK_SET);
		readed = fread(B, sizeof(double), n, fpB);
		//=========================================================================

		for (int i = 0; i < n; i++)
		{
			a = A[i];
			// Realiza a Multiplicação do elemento A pela linha B
			for (int j = 0; j < n; j++)
				C[i * n + j] += a * B[j];
		}
	}
	//---------------------------------------------------------------------------------

	// Join das matrizes calculadas
	double *result = (double *)calloc(matrixSize, sizeof(double));
	MPI_Reduce(C, result, matrixSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	free(C);
	//---------------------------------------------------------------------------------

	fclose(fpA);
	free(A);
	fclose(fpB);
	free(B);

	// SAÍDAS
	if (rank == 0)
	{
		// printLog(log_path, ALGORITMO, n, cpu_time, comun_cpu_time, exec_time, comun_time);
		if (output != 0)
		{
			printMatrix("output/C.txt", result, n);
		}
	}

	free(result);

	MPI_Finalize();

	return 0;
}
