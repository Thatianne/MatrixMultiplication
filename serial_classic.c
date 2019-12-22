#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define ALGORITMO "serial_classic"

typedef unsigned long int ulint;

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("Parametros invalidos, verifique...\n");
		return -1;
	}

	int n = atoi(argv[1]);

	char *path_matriz_A = argv[2];
	FILE *fpA = fopen(path_matriz_A, "rb");

	char *path_matriz_B = argv[3];
	FILE *fpB = fopen(path_matriz_B, "rb");

	size_t readed;

	ulint rowSize = (ulint)n * (ulint)sizeof(double);
	double *A = (double *)malloc(rowSize);
	double *B = (double *)malloc(rowSize);
	double *C = (double *)calloc((ulint)n * (ulint)n, sizeof(double));

	//---------------------------------------------------------------------------------
	for (int i = 0; i < n; i++)
	{
		//================================ LEITURA ================================
		// Lê a linha 'i' da matriz do arquivo 'fpA' e armazena em A
		fseek(fpA, 0, SEEK_SET);
		fseek(fpA, (ulint)i * (ulint)n * (ulint)sizeof(double), SEEK_SET);
		readed = fread(A, sizeof(double), n, fpA);
		//=========================================================================

		for (int j = 0; j < n; j++)
		{
			//================================ LEITURA ================================
			// Lê a coluna 'j' da matriz do arquivo 'fpB' e armazena em B
			fseek(fpB, 0, SEEK_SET);
			for (int x = 0; x < n; x++)
			{
				fseek(fpB, ((ulint)x * (ulint)n + (ulint)j) * (ulint)sizeof(double), SEEK_SET);
				readed = fread(&B[x], sizeof(double), 1, fpB);
			}
			//=========================================================================

			// Realiza a Multiplicação da linha A pela coluna B
			for (int k = 0; k < n; k++)
			{
				C[i * n + j] += A[k] * B[k];
				// printf("C(%d,%d) = A(%d,%d)*B(%d,%d) (%.f*%.f)\n", i, j, i, k, k, j, A[k], B[k]);
			}
			// printf("\n");
		}
	}
	//---------------------------------------------------------------------------------

	fclose(fpA);
	free(A);
	fclose(fpB);
	free(B);

	// SAÍDAS
	FILE *fp;
	fp = fopen("matrix/C.txt", "w+");
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
			fprintf(fp, "%lf ", C[i * n + j]);
		fprintf(fp, "\n");
	}
	fclose(fp);

	free(C);

	return 0;
}
