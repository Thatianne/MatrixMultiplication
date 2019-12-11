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

int *multiply(int *A, int *B, int n);
int *join(int *C11, int *C12, int *C21, int *C22, int n);

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
  int *C;

  int exec_start, exec_end, exec_time = 0.0;
  int cpu_start, cpu_end, cpu_time = 0.0;

  exec_start = curtime();
  cpu_start = ((int)(clock())) / CLOCKS_PER_SEC;

  //---------------------------------------------------------------------------------
  C = multiply(A, B, n);
  //---------------------------------------------------------------------------------

  exec_end = curtime();
  exec_time += (exec_end - exec_start);

  cpu_end = ((int)(clock())) / CLOCKS_PER_SEC;
  exec_end = curtime();

  cpu_time = (cpu_end - cpu_start);
  exec_time = (exec_end - exec_start);

  // SAÍDAS
  writeLog(logFile, ALGORITMO, n, cpu_time, exec_time);
  writeOutput(ALGORITMO, n, A, B, C);

  free(A);
  free(B);
  free(C);
}

int *multiply(int *A, int *B, int n)
{
  if (n == 1)
  {
    int *C = (int *)malloc(sizeof(int));
    C[0] = A[0] * B[0];
    return C;
  }

  int qSize = (n * n) / 4;

  int *A11 = (A + 0), *A12 = (A + qSize), *A21 = (A + qSize * 2), *A22 = (A + qSize * 3);
  int *B11 = (B + 0), *B12 = (B + qSize), *B21 = (B + qSize * 2), *B22 = (B + qSize * 3);

  int *S1 = (int *)malloc(sizeof(int) * qSize);
  int *S2 = (int *)malloc(sizeof(int) * qSize);
  int *S3 = (int *)malloc(sizeof(int) * qSize);
  int *S4 = (int *)malloc(sizeof(int) * qSize);
  int *S5 = (int *)malloc(sizeof(int) * qSize);
  int *S6 = (int *)malloc(sizeof(int) * qSize);
  int *S7 = (int *)malloc(sizeof(int) * qSize);
  int *S8 = (int *)malloc(sizeof(int) * qSize);
  int *S9 = (int *)malloc(sizeof(int) * qSize);
  int *S10 = (int *)malloc(sizeof(int) * qSize);

  int *P1, *P2, *P3, *P4, *P5, *P6, *P7;

  int *C11 = (int *)malloc(sizeof(int) * qSize);
  int *C12 = (int *)malloc(sizeof(int) * qSize);
  int *C21 = (int *)malloc(sizeof(int) * qSize);
  int *C22 = (int *)malloc(sizeof(int) * qSize);

  int x;
  for (x = 0; x < qSize; x++)
  {
    S1[x] = A11[x] + A22[x];  // S1 = A11 + A22
    S2[x] = B11[x] + B22[x];  // S2 = B11 + B22
    S3[x] = A21[x] + A22[x];  // S3 = A21 + A22
    S4[x] = B12[x] - B22[x];  // S4 = B12 - B22
    S5[x] = B21[x] - B11[x];  // S5 = B21 - B11
    S6[x] = A11[x] + A12[x];  // S6 = A11 + A12
    S7[x] = A21[x] - A11[x];  // S7 = A21 - A11
    S8[x] = B11[x] + B12[x];  // S8 = B11 + B12
    S9[x] = A12[x] - A22[x];  // S9 = A12 - A22
    S10[x] = B21[x] + B22[x]; // S10 = B21 + B22
  }

  P1 = multiply(S1, S2, n / 2);  // P1 = S1 * S2
  P2 = multiply(S3, B11, n / 2); // P2 = S3 * B11
  P3 = multiply(A11, S4, n / 2); // P3 = A11 * S4
  P4 = multiply(A22, S5, n / 2); // P4 = A22 * S5
  P5 = multiply(S6, B22, n / 2); // P5 = S6 * B22
  P6 = multiply(S7, S8, n / 2);  // P6 = S7 * S8
  P7 = multiply(S9, S10, n / 2); // P7 = S9 * S10

  for (x = 0; x < qSize; x++)
  {
    C11[x] = P1[x] + P4[x] - P5[x] + P7[x]; // C11 = P1 + P4 - P5 + P7
    C12[x] = P3[x] + P5[x];                 // C12 = P3 + P5;
    C21[x] = P2[x] + P4[x];                 // C21 = P2 + P4;
    C22[x] = P1[x] - P2[x] + P3[x] + P6[x]; // C22 = P1 - P2 + P3 + P6
  }

  return join(C11, C12, C21, C22, qSize);
}

// Admite que todas as matrizes tem dimensão (n/2 x n/2)
int *join(int *C11, int *C12, int *C21, int *C22, int n)
{
  int *C = (int *)malloc(n * n * sizeof(int));

  int _n = n / 2;
  int i1 = 0,
      i2 = _n,
      i3 = 0,
      i4 = _n;
  int j1, j2, j3, j4;

  int i, j;
  for (i = 0; i < _n; i++)
  {
    j1 = 0;
    j2 = 0;
    j3 = _n;
    j4 = _n;
    for (j = 0; j < _n; j++)
    {
      C[i1 * _n + j1] = C11[i * _n + j]; // Primeiro Quadrante
      C[i2 * _n + j2] = C12[i * _n + j]; // Segundo Quadrante
      C[i3 * _n + j3] = C21[i * _n + j]; // Terceiro Quadrante
      C[i4 * _n + j4] = C22[i * _n + j]; // Quarto Quadrante
    }
  }
  return C;
}