#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <x86intrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

struct timeval tv;
static double curtime(void)
{
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main(int argc, char *argv[])
{
  double start, end, function1_time = 0.0;
  /* Argumentos
    2 arquivos de entrada com as duas matrizes
    1 arquivo de saída
    dimensão da matriz
  */
  const char *matrix1Path = argv[1];
  const char *matrix2Path = argv[2];
  const char *matrix3Path = argv[3];
  int matrixDimension = atoi(argv[4]);
  int i, j, k, count = 0, sum, posM1, posM2, posM3, pSize, pot;
  int size = matrixDimension * matrixDimension;

  FILE *matrixFile1 = fopen(matrix1Path, "r");
  FILE *matrixFile2 = fopen(matrix2Path, "r");
  FILE *matrixFile3 = fopen(matrix3Path, "w");

  pot = log2(matrixDimension);

  if ((matrixDimension == 0) || ((matrixDimension & (matrixDimension - 1)) != 0))
  {
    printf("As matrizes devem ter dimensões em potência de 2\n");
    exit(0);
  }

  if (matrixFile1 == NULL)
  {
    printf("Não foi possível abrir o arquivo %s", argv[1]);
    exit(0);
  }
  if (matrixFile2 == NULL)
  {
    printf("Não foi possível abrir o arquivo %s", argv[2]);
    exit(0);
  }

  int long *matrix1 = (int long *)malloc(size * sizeof(int long));
  int long *matrix2 = (int long *)malloc(size * sizeof(int long));
  int long *matrix3 = (int long *)malloc(size * sizeof(int long));
  int long *p = (int long *)malloc((7 * pot) * sizeof(int long));

  while (!feof(matrixFile1) && !feof(matrixFile2))
  {
    fscanf(matrixFile1, "%ld", &*(matrix1 + count));
    fscanf(matrixFile2, "%ld", &*(matrix2 + count));
    count++;
  }

  start = curtime();

  *(p) = (*(matrix1 + ((0 * matrixDimension) + 0)) + *(matrix1 + ((1 * matrixDimension) + 1))) * (*(matrix2 + ((0 * matrixDimension) + 0)) + *(matrix2 + ((1 * matrixDimension) + 1)));
  *(p + 1) = *(matrix2 + ((0 * matrixDimension) + 0)) * (*(matrix1 + ((1 * matrixDimension) + 0)) + *(matrix1 + ((1 * matrixDimension) + 1)));
  *(p + 2) = *(matrix1 + ((0 * matrixDimension) + 0)) * (*(matrix2 + ((0 * matrixDimension) + 1)) - *(matrix2 + ((1 * matrixDimension) + 1)));
  *(p + 3) = *(matrix1 + ((1 * matrixDimension) + 1)) * (*(matrix2 + ((1 * matrixDimension) + 0)) - *(matrix2 + ((0 * matrixDimension) + 0)));
  *(p + 4) = *(matrix2 + ((1 * matrixDimension) + 1)) * (*(matrix1 + ((0 * matrixDimension) + 0)) + *(matrix1 + ((0 * matrixDimension) + 1)));
  *(p + 5) = (*(matrix1 + ((1 * matrixDimension) + 0)) - *(matrix1 + ((0 * matrixDimension) + 0))) * (*(matrix2 + ((0 * matrixDimension) + 0)) + *(matrix2 + ((0 * matrixDimension) + 1)));
  *(p + 6) = (*(matrix1 + ((0 * matrixDimension) + 1)) - *(matrix1 + ((1 * matrixDimension) + 1))) * (*(matrix2 + ((1 * matrixDimension) + 0)) + *(matrix2 + ((1 * matrixDimension) + 1)));

  *(matrix3) = *(p) + *(p + 3) - *(p + 4) + *(p + 6);
  *(matrix3 + 1) = *(p + 2) + *(p + 4);
  *(matrix3 + 2) = *(p + 1) + *(p + 3);
  *(matrix3 + 3) = *(p) - *(p + 1) + *(p + 2) + *(p + 5);

  end = curtime();
  function1_time += (end - start);

  printf("Tempo de duração %f segundos\n", function1_time);
  fprintf(matrixFile3, "%ld\t", *(matrix3));
  fprintf(matrixFile3, "%ld", *(matrix3 + 1));
  fprintf(matrixFile3, "\n");
  fprintf(matrixFile3, "%ld\t", *(matrix3 + 2));
  fprintf(matrixFile3, "%ld", *(matrix3 + 3));

  fclose(matrixFile1);
  fclose(matrixFile2);
  fclose(matrixFile3);

  free(matrix1);
  free(matrix2);
  free(matrix3);
  free(p);
}