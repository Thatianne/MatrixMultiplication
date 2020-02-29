#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <stdio.h>
#include <stdlib.h>
#include "util.c"

#include <time.h>
#include <sys/time.h>

#define ALGORITMO "summa_serial"

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

	// LOG
	clock_t start, end, read_start, read_end;
	struct timeval exec_t1, exec_t2;
	struct timeval read_t1, read_t2;
	double exec_time = 0, read_time = 0, cpu_time = 0, read_cpu_time = 0;
	//---------------------------------------------------------------------------------

	gettimeofday(&exec_t1, NULL);
	start = clock();

	for (int k = 0; k < n; k++)
	{
		//================================ LEITURA ================================
		gettimeofday(&read_t1, NULL);
		read_start = clock();

		// Lê a coluna 'k' da matriz do arquivo 'fpA' e armazena em A
		fseek(fpA, 0, SEEK_SET);
		fseek(fpA, ((ulint)k * n) * (ulint)sizeof(double), SEEK_SET);
		readed = fread(A, sizeof(double), n, fpA);

		// Lê a linha 'k' da matriz do arquivo 'fpB' e armazena em B
		fseek(fpB, 0, SEEK_SET);
		fseek(fpB, ((ulint)k * n) * (ulint)sizeof(double), SEEK_SET);
		readed = fread(B, sizeof(double), n, fpB);

		read_end = clock();
		gettimeofday(&read_t2, NULL);
		read_time += getDiffTime(read_t1, read_t2);
		read_cpu_time += ((double)(read_end - read_start)) / CLOCKS_PER_SEC;
		//=========================================================================

		for (int i = 0; i < n; i++)
		{
			a = A[i];
			// Realiza a Multiplicação do elemento A pela linha B
			for (int j = 0; j < n; j++)
				C[i * n + j] = C[i * n + j] + (a * B[j]);
		}
	}
	//---------------------------------------------------------------------------------

	end = clock();
	gettimeofday(&exec_t2, NULL);
	cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	exec_time += getDiffTime(exec_t1, exec_t2);

	// SAÍDAS
	printLog(log_path, ALGORITMO, n, exec_time, cpu_time, read_time, read_cpu_time);
	if (output != 0)
	{
		printMatrix("output/C_summa_serial.txt", C, n);
	}

	fclose(fpA);
	fclose(fpB);
	free(B);
	free(C);
	
	printf("done\n");

	return 0;
}
