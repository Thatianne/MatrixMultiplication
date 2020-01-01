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
  ulint matrixSize = (ulint)n * (ulint)n;
  double A;
  double *B = (double *)malloc(rowSize);
  double *C = (double *)calloc(matrixSize, sizeof(double));
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
        C[i * n + j] += A * B[j];
    }
  }
  //---------------------------------------------------------------------------------

  // Join das matrizes calculadas
  double *result = (double *)calloc(matrixSize, sizeof(double));
  MPI_Reduce(C, result, matrixSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  //---------------------------------------------------------------------------------

  fclose(fpA);
  fclose(fpB);
  free(B);

  // SAÍDAS
  if (output != 0 && rank == 0)
  {
    FILE *fpC;
    fpC = fopen("matrix/C.txt", "w+");
    for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < n; j++)
        fprintf(fpC, "%lf ", result[i * n + j]);
      fprintf(fpC, "\n");
    }
    fclose(fpC);
  }

  free(C);

  MPI_Finalize();

  return 0;
}
