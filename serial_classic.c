#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "utils.c"

#define ALGORITMO "serial_classic"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Parametros invalidos, verifique...\n");
		return -1;
	}

	int size = atoi(argv[1]);
	char *logFile = argv[2];

	// CRIAÇÃO DE MATRIZES
	int *A = createMatrix(size, 0);
	int *B = createMatrix(size, 1);
	int *C = malloc(size * size * sizeof(int));

	// TEMPOS
	clock_t cpu_t1 = clock();
	struct timeval exec_t1;
	gettimeofday(&exec_t1, NULL);

	// PROCESSAMENTO
	int m, n, x;
	for (m = 0; m < size; m++)
		for (n = 0; n < size; n++)
			for (x = 0; x < size; x++)
				*(getAddress(C, size, m, n)) += *(getAddress(A, size, m, x)) * *(getAddress(B, size, x, n));

	// SAÍDAS
	writeOutput(ALGORITMO, size, A, B, C);

	clock_t cpu_t2 = clock();
	double cpu_time = ((double)(cpu_t2 - cpu_t1)) / CLOCKS_PER_SEC;
	struct timeval exec_t2;
	gettimeofday(&exec_t2, NULL);
	double exec_time = (double)(exec_t2.tv_usec - exec_t1.tv_usec) / 1000000 + (double)(exec_t2.tv_sec - exec_t1.tv_sec);
	writeLog(logFile, ALGORITMO, size, cpu_time, exec_time);

	return 0;
}
