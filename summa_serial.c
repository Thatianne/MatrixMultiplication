#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define ALGORITMO "summa_serial"

typedef unsigned long int ulint;

int main(int argc, char *argv[])
{
  if (argc < 4)
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

  int output = (argc > 4) ? atoi(argv[4]) : 1;

  size_t readed;

  ulint rowSize = (ulint)n * (ulint)sizeof(double);
  double A;
  double *B = (double *)malloc(rowSize);
  double *C = (double *)calloc((ulint)n * (ulint)n, sizeof(double));
  //---------------------------------------------------------------------------------

  for (int k = 0; k < n; k++)
  {
    //================================ LEITURA ================================
    // Lê a linha 'k' da matriz do arquivo 'fpB' e armazena em B
    fseek(fpB, 0, SEEK_SET);
    fseek(fpB, ((ulint)k * (ulint)n) * (ulint)sizeof(double), SEEK_SET);
    readed = fread(B, sizeof(double), n, fpB);
    //=========================================================================

    for (int i = 0; i < n; i++)
    {
      //================================ LEITURA ================================
      // Lê o elemento A(i,k) da matriz do arquivo 'fpA' e armazena em A
      fseek(fpA, 0, SEEK_SET);
      fseek(fpA, ((ulint)i * (ulint)n + (ulint)k) * (ulint)sizeof(double), SEEK_SET);
      readed = fread(&A, sizeof(double), 1, fpA);
      //=========================================================================

      for (int j = 0; j < n; j++)
      {
        C[i * n + j] += A * B[j];
      }
    }
  }
  //---------------------------------------------------------------------------------

  fclose(fpA);
  fclose(fpB);
  free(B);

  // SAÍDAS
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
