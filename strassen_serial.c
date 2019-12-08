#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <x86intrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "utils.c"

#define ALGORITMO "strassen_serial"

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Parametros invalidos, verifique...\n");
    return -1;
  }

  int dimension = atoi(argv[1]);
  char *logFile = argv[2];

  // CRIAÇÃO DE MATRIZES
  int *A = createMatrix(dimension, 0);
  int *B = createMatrix(dimension, 1);
  int size = dimension * dimension;
  int *C = malloc(size * sizeof(int));

  double exec_start, exec_end, exec_time = 0.0;
  double cpu_start, cpu_end, cpu_time = 0.0;

  exec_start = curtime();
  cpu_start = ((double)(clock())) / CLOCKS_PER_SEC;

  //---------------------------------------------------------------------------------
  int i, j, k, count = 0, sum, posM1, posM2, posM3, pSize, pot;

  pot = log2(dimension);
  int long *p = (int long *)malloc((7 * pot) * sizeof(int long));

  *(p) = (*(A + ((0 * dimension) + 0)) + *(A + ((1 * dimension) + 1))) * (*(B + ((0 * dimension) + 0)) + *(B + ((1 * dimension) + 1)));
  *(p + 1) = *(B + ((0 * dimension) + 0)) * (*(A + ((1 * dimension) + 0)) + *(A + ((1 * dimension) + 1)));
  *(p + 2) = *(A + ((0 * dimension) + 0)) * (*(B + ((0 * dimension) + 1)) - *(B + ((1 * dimension) + 1)));
  *(p + 3) = *(A + ((1 * dimension) + 1)) * (*(B + ((1 * dimension) + 0)) - *(B + ((0 * dimension) + 0)));
  *(p + 4) = *(B + ((1 * dimension) + 1)) * (*(A + ((0 * dimension) + 0)) + *(A + ((0 * dimension) + 1)));
  *(p + 5) = (*(A + ((1 * dimension) + 0)) - *(A + ((0 * dimension) + 0))) * (*(B + ((0 * dimension) + 0)) + *(B + ((0 * dimension) + 1)));
  *(p + 6) = (*(A + ((0 * dimension) + 1)) - *(A + ((1 * dimension) + 1))) * (*(B + ((1 * dimension) + 0)) + *(B + ((1 * dimension) + 1)));

  *(C) = *(p) + *(p + 3) - *(p + 4) + *(p + 6);
  *(C + 1) = *(p + 2) + *(p + 4);
  *(C + 2) = *(p + 1) + *(p + 3);
  *(C + 3) = *(p) - *(p + 1) + *(p + 2) + *(p + 5);
  //---------------------------------------------------------------------------------

  exec_end = curtime();
  exec_time += (exec_end - exec_start);

  cpu_end = ((double)(clock())) / CLOCKS_PER_SEC;
  exec_end = curtime();

  cpu_time = (cpu_end - cpu_start);
  exec_time = (exec_end - exec_start);

  // SAÍDAS
	writeLog(logFile, ALGORITMO, dimension, cpu_time, exec_time);
	writeOutput(ALGORITMO, dimension, A, B, C);

  free(A);
  free(B);
  free(C);
  free(p);
}