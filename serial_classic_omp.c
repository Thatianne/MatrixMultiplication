#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "util.c"

#define ALGORITMO "serial_classic"

int main(int argc, char *argv[])
{
	if (argc < 5)
	{
		printf("Parametros invalidos, verifique...\n");
		return -1;
	}

	int n = atoi(argv[1]);

	char *path_matriz_A = argv[2];
	FILE *fpA = fopen(path_matriz_A, "rb");

	char *path_matriz_B = argv[3];
	FILE *fpB = fopen(path_matriz_B, "rb");

	const char *log_path = argv[4];

	int output = (argc > 5) ? atoi(argv[5]) : 1;

	size_t readed;

	ulint rowSize = (ulint)n * (ulint)sizeof(double);
	double *A = (double *)malloc(rowSize);
	double *B = (double *)malloc(rowSize);
	double *C = (double *)calloc((ulint)n * (ulint)n, sizeof(double));

	// LOG
	clock_t start, end, comun_start, comun_end;
	struct timeval exec_t1, exec_t2;
	struct timeval comun_t1, comun_t2;
	double exec_time = 0, comun_time = 0, cpu_time = 0, comun_cpu_time = 0;
	//---------------------------------------------------------------------------------

	gettimeofday(&exec_t1, NULL);
	start = clock();

#pragma omp parallel for shared(path_matriz_A, path_matriz_B, C, n, rowSize) private(i, fpA, A, fpB, B, readed) schedule(dynamic)
	for (int i = 0; i < n; i++)
	{
		//================================ LEITURA ================================
		gettimeofday(&comun_t1, NULL);
		comun_start = clock();

		// Lê a linha 'i' da matriz do arquivo 'fpA' e armazena em A
		fseek(fpA, 0, SEEK_SET);
		fseek(fpA, (ulint)i * (ulint)n * (ulint)sizeof(double), SEEK_SET);
		readed = fread(A, sizeof(double), n, fpA);

		comun_end = clock();
		gettimeofday(&comun_t2, NULL);
		comun_time += getDiffTime(comun_t1, comun_t2);
		comun_cpu_time += ((double)(comun_end - comun_start)) / CLOCKS_PER_SEC;
		//=========================================================================

#pragma omp parallel for shared(path_matriz_A, path_matriz_B, C, n, i, fpB, B) private(j, fpA, A, readed) schedule(dynamic)
		for (int j = 0; j < n; j++)
		{
			//================================ LEITURA ================================
			gettimeofday(&comun_t1, NULL);
			comun_start = clock();

			// Lê a coluna 'j' da matriz do arquivo 'fpB' e armazena em B
			fseek(fpB, 0, SEEK_SET);
			for (int x = 0; x < n; x++)
			{
				fseek(fpB, ((ulint)x * (ulint)n + (ulint)j) * (ulint)sizeof(double), SEEK_SET);
				readed = fread(&B[x], sizeof(double), 1, fpB);
			}

			comun_end = clock();
			gettimeofday(&comun_t2, NULL);
			comun_cpu_time += ((double)(comun_end - comun_start)) / CLOCKS_PER_SEC;
			comun_time += getDiffTime(comun_t1, comun_t2);
			//=========================================================================

			// Realiza a Multiplicação da linha A pela coluna B
#pragma omp parallel for shared(C, n, A, B, i, j) private(k) schedule(dynamic)
			for (int k = 0; k < n; k++)
				C[i * n + j] += A[k] * B[k];
		}
	}
	//---------------------------------------------------------------------------------

	end = clock();
	gettimeofday(&exec_t2, NULL);
	cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	exec_time += getDiffTime(exec_t1, exec_t2);

	fclose(fpA);
	free(A);
	fclose(fpB);
	free(B);

	// SAÍDAS
	FILE *log;
	log = fopen(log_path, "a");
	fprintf(log, "%s,%d,%f,%f,%f,%f\n", ALGORITMO, n, (cpu_time - comun_cpu_time), (exec_time - comun_time), comun_time, comun_cpu_time);
	fclose(log);

	if (output != 0)
	{
		FILE *fpC;
		fpC = fopen("matrix/C.txt", "w+");
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
				fprintf(fpC, "%lf ", C[i * n + j]);
			fprintf(fpC, "\n");
		}
		fclose(fpC);
	}

	free(C);

	return 0;
}
