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
	if (argc != 3)
	{
		printf("Parametros invalidos, verifique...\n");
		return -1;
	}

	int n = atoi(argv[1]);
	char *logFile = argv[2];
	char *path_matriz = argv[3];
	FILE *fpA;
	FILE *fpB;
	fpA = fopen(path_matriz, "rb");

	//criação das matrizes

	double *A;
	double *B;
	double *C;

	//---------------------------------------------------------------------------------
	// Configurações do MPI
	MPI_Init(&argc, &argv);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char mpi_processor_name[MPI_MAX_PROCESSOR_NAME];
	int mpi_name_len;
	MPI_Get_processor_name(mpi_processor_name, &mpi_name_len);

	MPI_Status status;

	//tamanho das matrizes A e B	LINHA/COLUNAS		TAMANHO
	ulint tamanho = (ulint)n * (ulint)sizeof(double);
	//tamanho da matriz c		LINHAS							COLUNAS						PROFUNDIDADE
	ulint tamanho2 = (ulint)n * (ulint)n * (ulint)sizeof(double);

	A = (double *)malloc(tamanho);
	B = (double *)malloc(tamanho);
	C = (double *)malloc(tamanho2);

	// admitindo que rank começa em 0 e tambem que o processo MASTER trabala igualmente
	for (int col_a = rank; col_a < n; col_a += (world_size))
	{
		ulint p1 = (ulint)n * (ulint)n * (ulint)col_a;
		//salva a coluna da matriz A na variável 'A'
		fseek(fpA, 0, SEEK_SET);
		for (int i = 0; i < n; i++)
		{
			fseek(fpA, i * n * sizeof(double) + col_a * sizeof(double), SEEK_SET);
			fread(&A[i], sizeof(double), 1, fpA);
			fseek(fpA, 0, SEEK_SET);
		}

		//Pega as linhas B
		for (ulint linha = 0; linha < n; linha++)
		{
			fseek(fpB, (ulint)linha * (ulint)n * (ulint)sizeof(double), SEEK_SET);
			fread(B, sizeof(double), n, fpB);
			fseek(fpB, 0, SEEK_SET);

			//realiza a conta para cada coluna e cada linha
			for (int i = 0; i < n; i++)
			{
				ulint p2 = (ulint)i * (ulint)n;
				for (int j = 0; j < n; j++)
				{
					C[p1 + p2 + j] = A[i] * C[j];
				}
			}
		}
	}
}

MPI_Finalize();
//---------------------------------------------------------------------------------

return 0;
}
