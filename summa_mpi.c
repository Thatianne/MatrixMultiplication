#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>
#include <sys/time.h>
#include "utils.c"

#define ALGORITMO "summa_mpi"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Parametros invalidos, verifique...\n");
		return -1;
	}

	int n = atoi(argv[1]);
	char *logFile = argv[2];

	// CRIAÇÃO DE MATRIZES
	int *A = createMatrix(n, 0);
	int *B = createMatrix(n, 1);
	int size = n * n;
	int *C = (int *)malloc(size * sizeof(int));

	double exec_start, exec_end, exec_time = 0.0;
	double cpu_start, cpu_end, cpu_time = 0.0;

	exec_start = curtime();
	cpu_start = ((double)(clock())) / CLOCKS_PER_SEC;

	//---------------------------------------------------------------------------------
	// Configurações do MPI
	MPI_Init(&argc, &argv);

	int mpi_world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_world_size);

	int mpi_world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_world_rank);

	char mpi_processor_name[MPI_MAX_PROCESSOR_NAME];
	int mpi_name_len;
	MPI_Get_processor_name(mpi_processor_name, &mpi_name_len);

	int i, j, k;
	for (k = 0; k < n; k++)
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				C[n * i + j] += A[n * i + k] * B[n * k + j];

	MPI_Finalize();
	//---------------------------------------------------------------------------------

	cpu_end = ((double)(clock())) / CLOCKS_PER_SEC;
	exec_end = curtime();

	cpu_time = (cpu_end - cpu_start);
	exec_time = (exec_end - exec_start);

	// SAÍDAS
	writeLog(logFile, ALGORITMO, n, cpu_time, exec_time);
	writeOutput(ALGORITMO, n, A, B, C);

	free(A);
	free(B);
	free(C);

	return 0;
}
