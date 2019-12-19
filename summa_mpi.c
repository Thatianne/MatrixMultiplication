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
#define MASTER 0
#define ENVIO_OFFSET 1000
#define ENVIO_VALOR_INICIAL 2000

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

	//tamanho das matrizes A e B	LINHA/COLUNAS		TAMANHO
	ulint rowSize = (ulint)n * (ulint)sizeof(double);
	ulint matrixSize = (ulint)n * rowSize;
	double *A = (double *)malloc(rowSize);
	double *B = (double *)malloc(rowSize);
	double *C = (double *)malloc(matrixSize);

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

	// admite que rank começa em 0 e que o processo MASTER trabalha igualmente
	for (int col_a = rank; col_a < n; col_a += (world_size))
	{
		// Lê a coluna 'col_a' do arquivo 'fpA' e armazena em A
		fseek(fpA, (ulint)col_a * (ulint)n * (ulint)sizeof(double), SEEK_SET);
		fread(A, sizeof(double), n, fpA);
		fseek(fpA, 0, SEEK_SET);

		for (ulint row = 0; row < n; row++)
		{
			// Lê a linha 'row' do arquivo 'fpB' e armazena em B
			fseek(fpB, (ulint)row * (ulint)n * (ulint)sizeof(double), SEEK_SET);
			fread(B, sizeof(double), n, fpB);
			fseek(fpB, 0, SEEK_SET);

			// Realiza a Multiplicação da coluna A pela Linha B
			for (int i = 0; i < n; i++)
				for (int j = 0; j < n; j++)
					C[i * n + j] += A[i] * B[j];
		}
	}

	MPI_Finalize();

	FILE *fp;
	fp = fopen("matrix/C.txt", "w+");
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
			fprintf(fp, "%d ", C[i * n + j]);
		fprintf(fp, "\n");
	}
	fclose(fp);
	//---------------------------------------------------------------------------------

	return 0;
}
