#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx2")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mpi.h"
#include <omp.h>
#include "util.c"

#define ALGORITMO "summa_mpi_omp"

int main(int argc, char *argv[])
{
	if (argc < 5)
	{
		printf("Parametros invalidos (%d), verifique...\n", argc);
		return -1;
	}

	int nThreads = atoi(argv[1]);
	int n = atoi(argv[2]);

	char *path_matriz_A = argv[3];
	FILE *fpA;

	char *path_matriz_B = argv[4];
	FILE *fpB;

	int output = (argc > 5) ? atoi(argv[5]) : 1;

	size_t readed;

	ulint rowSize = (ulint)n * (ulint)sizeof(double);
	ulint matrixSize = (ulint)n * (ulint)n;
	double A;
	double *B;
	double *C = (double *)calloc(matrixSize, sizeof(double));
	//---------------------------------------------------------------------------------

	// Configurações do OpenMP
	omp_set_num_threads(nThreads);
	omp_set_dynamic(0);
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
	//---------------------------------------------------------------------------------

	int k;
#pragma omp parallel for shared(path_matriz_A, path_matriz_B, C, n, rank, rowSize, world_size) private(k, fpA, A, fpB, B, readed) schedule(dynamic)
	for (k = rank; k < n; k += (world_size))
	{
		//================================ LEITURA ================================
		// Lê a linha 'k' da matriz do arquivo 'fpB' e armazena em B
		fpB = fopen(path_matriz_B, "rb");
		B = (double *)malloc(rowSize);
		fseek(fpB, 0, SEEK_SET);
		fseek(fpB, ((ulint)k * (ulint)n) * (ulint)sizeof(double), SEEK_SET);
		readed = fread(B, sizeof(double), n, fpB);
		//=========================================================================

		int i;
#pragma omp parallel for shared(path_matriz_A, path_matriz_B, C, n, k, fpB, B) private(i, fpA, A, readed) schedule(dynamic)
		for (i = 0; i < n; i++)
		{
			//================================ LEITURA ================================
			// Lê o elemento A(i,k) da matriz do arquivo 'fpA' e armazena em A
			fpA = fopen(path_matriz_A, "rb");
			fseek(fpA, 0, SEEK_SET);
			fseek(fpA, ((ulint)i * (ulint)n + (ulint)k) * (ulint)sizeof(double), SEEK_SET);
			readed = fread(&A, sizeof(double), 1, fpA);
			//=========================================================================

			int j;
#pragma omp parallel for shared(C, n, A, B, i) private(j) schedule(dynamic)
			// Realiza a Multiplicação de A pela linha B
			for (j = 0; j < n; j++)
				C[i * n + j] += A * B[j];

			fclose(fpA);
		}
		fclose(fpB);
		free(B);
	}
	//---------------------------------------------------------------------------------

	// Join das matrizes calculadas
	double *result = (double *)calloc(matrixSize, sizeof(double));
	MPI_Reduce(C, result, n * n, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	//---------------------------------------------------------------------------------

	// SAÍDAS
	if (rank == 0)
	{
		// printLog(log_path, ALGORITMO, n, cpu_time, comun_cpu_time, exec_time, comun_time);
		if (output != 0)
		{
			printMatrix("matrix/C.txt", C, n);
		}
	}

	free(C);

	MPI_Finalize();

	return 0;
}
