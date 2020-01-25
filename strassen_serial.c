#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define ALGORITMO "strassen_serial"

typedef unsigned long int ulint;

void fileToMatrix(char fileName[], double arr[], int size);
void printMatrix(double arr[], int size);
void sum(double a[], double b[], double c[], int size);
void subtract(double a[], double b[], double c[], int size);
void baseAlgorithm(double a[], double b[], double c[], int size);
void strassen(double a[], double b[], double c[], int size);
void matrixToFile(char filename[], double arr[], int size);

int main(int argc, char *argv[])
{
  if (argc < 4)
  {
    printf("Parametros invalidos, verifique...\n");
    return -1;
  }

  int n = atoi(argv[1]);
  char *path_matriz_A = argv[2];
  char *path_matriz_B = argv[3];

  int output = (argc > 4) ? atoi(argv[4]) : 1;

  double *matrixA = (double *)malloc((ulint)n * (ulint)n * (ulint)sizeof(double));
  double *matrixB = (double *)malloc((ulint)n * (ulint)n * (ulint)sizeof(double));
  double *matrixC = (double *)malloc((ulint)n * (ulint)n * (ulint)sizeof(double));

  fileToMatrix(path_matriz_A, matrixA, n);
  fileToMatrix(path_matriz_B, matrixB, n);

  strassen(matrixA, matrixB, matrixC, n);

  if (output == 1)
  {
    matrixToFile("matrix/strassen_C.txt", matrixC, n);
  }
  printMatrix(matrixC, n);
}

void fileToMatrix(char filename[], double arr[], int size)
{
  size_t readed;
  FILE *file = fopen(filename, "rb");
  fseek(file, 0, SEEK_SET);
  for (int i = 0; i < size; i++)
  {
    fseek(file, ((ulint)i * (ulint)size) * (ulint)sizeof(double), SEEK_SET);
    readed = fread(&arr[i * size], sizeof(double), size, file);
  }
}

void printMatrix(double arr[], int size)
{
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      printf("%f\t", arr[i * size + j]);
    }
    printf("\n");
  }
}

void baseAlgorithm(double a[], double b[], double c[], int size)
{
  double long *p = (double long *)malloc(7 * sizeof(double long));
  p[0] = (a[0 * size + 0] + a[1 * size + 1]) * (b[0 * size + 0] + b[1 * size + 1]);
  p[1] = b[0 * size + 0] * (a[1 * size + 0] + a[1 * size + 1]);
  p[2] = a[0 * size + 0] * (b[0 * size + 1] - b[1 * size + 1]);
  p[3] = a[1 * size + 1] * (b[1 * size + 0] - b[0 * size + 0]);
  p[4] = b[1 * size + 1] * (a[0 * size + 0] + a[0 * size + 1]);
  p[5] = (a[1 * size + 0] - a[0 * size + 0]) * (b[0 * size + 0] + b[0 * size + 1]);
  p[6] = (a[0 * size + 1] - a[1 * size + 1]) * (b[1 * size + 0] + b[1 * size + 1]);

  c[0] = p[0] + p[3] - p[4] + p[6];
  c[1] = p[2] + p[4];
  c[2] = p[1] + p[3];
  c[3] = p[0] - p[1] + p[2] + p[5];
}

void strassen(double a[], double b[], double c[], int size)
{
  if (size <= 2)
  {
    baseAlgorithm(a, b, c, size);
  }
  else
  {

    int newSize = size / 2;
    ulint mallocSize = (ulint)newSize * (ulint)newSize * (ulint)sizeof(double);

    double *a11 = (double *)malloc(mallocSize);
    double *a12 = (double *)malloc(mallocSize);
    double *a21 = (double *)malloc(mallocSize);
    double *a22 = (double *)malloc(mallocSize);

    double *b11 = (double *)malloc(mallocSize);
    double *b12 = (double *)malloc(mallocSize);
    double *b21 = (double *)malloc(mallocSize);
    double *b22 = (double *)malloc(mallocSize);

    double *c11 = (double *)malloc(mallocSize);
    double *c12 = (double *)malloc(mallocSize);
    double *c21 = (double *)malloc(mallocSize);
    double *c22 = (double *)malloc(mallocSize);

    double *p1 = (double *)malloc(mallocSize);
    double *p2 = (double *)malloc(mallocSize);
    double *p3 = (double *)malloc(mallocSize);
    double *p4 = (double *)malloc(mallocSize);
    double *p5 = (double *)malloc(mallocSize);
    double *p6 = (double *)malloc(mallocSize);
    double *p7 = (double *)malloc(mallocSize);

    double *aResult = (double *)malloc(mallocSize);
    double *bResult = (double *)malloc(mallocSize);

    int pos;

    for (int i = 0; i < newSize; i++)
    {
      for (int j = 0; j < newSize; j++)
      {
        pos = i * newSize + j;

        a11[pos] = a[i * size + j];
        a12[pos] = a[(i * size) + (j + newSize)];
        a21[pos] = a[(i + newSize) * size + j];
        a22[pos] = a[(i + newSize) * size + (j + newSize)];

        b11[pos] = b[i * size + j];
        b12[pos] = b[(i * size) + (j + newSize)];
        b21[pos] = b[(i + newSize) * size + j];
        b22[pos] = b[(i + newSize) * size + (j + newSize)];
      }
    }

    sum(a11, a22, aResult, newSize); // a11 + a22
    sum(b11, b22, bResult, newSize); // b11 + b22

    strassen(aResult, bResult, p1, newSize); // p1 = (a11+a22) * (b11+b22)

    sum(a21, a22, aResult, newSize);     // a21 + a22
    strassen(aResult, b11, p2, newSize); // p2 = (a21+a22) * (b11)

    subtract(b12, b22, bResult, newSize); // b12 - b22
    strassen(a11, bResult, p3, newSize);  // p3 = (a11) * (b12 - b22)

    subtract(b21, b11, bResult, newSize); // b21 - b11
    strassen(a22, bResult, p4, newSize);  // p4 = (a22) * (b21 - b11)

    sum(a11, a12, aResult, newSize);     // a11 + a12
    strassen(aResult, b22, p5, newSize); // p5 = (a11+a12) * (b22)

    subtract(a21, a11, aResult, newSize);    // a21 - a11
    sum(b11, b12, bResult, newSize);         // b11 + b12
    strassen(aResult, bResult, p6, newSize); // p6 = (a21-a11) * (b11+b12)

    subtract(a12, a22, aResult, newSize);    // a12 - a22
    sum(b21, b22, bResult, newSize);         // b21 + b22
    strassen(aResult, bResult, p7, newSize); // p7 = (a12-a22) * (b21+b22)

    sum(p3, p5, c12, newSize); // c12 = p3 + p5
    sum(p2, p4, c21, newSize); // c21 = p2 + p4

    sum(p1, p4, aResult, newSize);       // p1 + p4
    sum(aResult, p7, bResult, newSize);  // p1 + p4 + p7
    subtract(bResult, p5, c11, newSize); // c11 = p1 + p4 - p5 + p7

    sum(p1, p3, aResult, newSize);       // p1 + p3
    sum(aResult, p6, bResult, newSize);  // p1 + p3 + p6
    subtract(bResult, p2, c22, newSize); // c22 = p1 + p3 - p2 + p6

    for (int i = 0; i < newSize; i++)
    {
      for (int j = 0; j < newSize; j++)
      {
        c[i * size + j] = c11[i * newSize + j];
        c[i * size + (j + newSize)] = c12[i * newSize + j];
        c[(i + newSize) * size + j] = c21[i * newSize + j];
        c[(i + newSize) * size + (j + newSize)] = c22[i * newSize + j];
      }
    }
  }
}

void sum(double a[], double b[], double c[], int size)
{
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      c[i * size + j] = a[i * size + j] + b[i * size + j];
    }
  }
}

void subtract(double a[], double b[], double c[], int size)
{
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      c[i * size + j] = a[i * size + j] - b[i * size + j];
    }
  }
}

void matrixToFile(char filename[], double arr[], int size)
{
  FILE *file;
  file = fopen(filename, "w+");
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
      fprintf(file, "%lf ", arr[i * size + j]);
    fprintf(file, "\n");
  }
  fclose(file);
}