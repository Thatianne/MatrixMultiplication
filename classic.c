#include <stdio.h>
#include <stdlib.h>

/* Apenas para validar os resultados dos outros algoritmos
*/
int main(int argc, char *argv[])
{
  /* Argumentos
    2 arquivos de entrada com as duas matrizes
    1 arquivo de saída
    dimensão da matriz
  */
  const char *matrix1Path = argv[1];
  const char *matrix2Path = argv[2];
  const char *matrix3Path = argv[3];
  int matrixDimension = atoi(argv[4]);
  int i, j, k, count = 0, sum, posM1, posM2, posM3;
  int size = matrixDimension * matrixDimension;

  FILE *matrixFile1 = fopen(matrix1Path, "r");
  FILE *matrixFile2 = fopen(matrix2Path, "r");
  FILE *matrixFile3 = fopen(matrix3Path, "w");

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

  while (!feof(matrixFile1) && !feof(matrixFile2))
  {
    fscanf(matrixFile1, "%ld", &*(matrix1 + count));
    fscanf(matrixFile2, "%ld", &*(matrix2 + count));
    count++;
  }

  for (i = 0; i < matrixDimension; i++)
  {
    for (j = 0; j < matrixDimension; j++)
    {
      sum = 0;
      for (k = 0; k < matrixDimension; k++)
      {
        posM1 = (i * matrixDimension) + k;
        posM2 = (k * matrixDimension) + j;
        sum += *(matrix1 + posM1) * *(matrix2 + posM2);
      }

      posM3 = (i * matrixDimension) + j;
      *(matrix3 + posM3) = sum;
      fprintf(matrixFile3, "%ld\t", sum);
    }
    fprintf(matrixFile3, "\n");
  }

  fclose(matrixFile1);
  fclose(matrixFile2);
  fclose(matrixFile3);

  free(matrix1);
  free(matrix2);
  free(matrix3);
}