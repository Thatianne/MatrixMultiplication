#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mpi.h"

#define ALGORITMO "summa_mpi"

typedef unsigned long int ulint;

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("Parametros invalidos, verifique...\n");
		return -1;
	}

	int n = atoi(argv[1]);
	FILE *fpA;
	char *path_matriz_A = argv[2];
	fpA = fopen(path_matriz_A, "rb");

	FILE *fpB;
	char *path_matriz_B = argv[3];
	fpB = fopen(path_matriz_B, "rb");

	size_t readed;

	ulint rowSize = (ulint)n * (ulint)sizeof(double);
	double *A = (double *)malloc(rowSize);
	double *B = (double *)malloc(rowSize);
	double *C = (double *)malloc((ulint)n * rowSize);

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
		for (int i = 0; i < n; i++)
		{
			//================================ LEITURA ================================
			// Lê o elemento A(i,k) da matriz do arquivo 'fpA' e armazena em A
			fseek(fpA, 0, SEEK_SET);
			fseek(fpA, ((ulint)i * (ulint)n + (ulint)k) * (ulint)sizeof(double), SEEK_SET);
			readed = fread(&A[0], sizeof(double), 1, fpA);
			//=========================================================================

			//================================ LEITURA ================================
			// Lê a linha 'k' da matriz do arquivo 'fpB' e armazena em B
			fseek(fpB, 0, SEEK_SET);
			fseek(fpB, ((ulint)k * (ulint)n) * (ulint)sizeof(double), SEEK_SET);
			readed = fread(B, sizeof(double), n, fpB);
			//=========================================================================

			for (int j = 0; j < n; j++)
			{
				// Realiza a Multiplicação de A pela linha B
				C[i * n + j] += A[0] * B[j];
				// printf("C(%d,%d) = A(%d,%d)*B(%d,%d) (%.f*%.f)\n", i, j, i, k, k, j, A[0], B[j]);
			}
			// printf("\n");
		}
	}
	//---------------------------------------------------------------------------------
	// Join das matrizes calculadas
	double *result = (double *)malloc((ulint)n * rowSize);
	MPI_Reduce(C, result, n * n, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	//---------------------------------------------------------------------------------

	// SAÍDAS
	if (rank == 0)
	{
		FILE *fp;
		fp = fopen("matrix/C.txt", "w+");
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
				fprintf(fp, "%lf ", result[i * n + j]);
			fprintf(fp, "\n");
		}
		fclose(fp);
	}

	free(A);
	free(B);
	free(C);

	MPI_Finalize();

	return 0;
}
