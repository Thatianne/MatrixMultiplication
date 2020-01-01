#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mpi.h"

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

  // Configurações do MPI
  MPI_Init(&argc, &argv);

  fileToMatrix(path_matriz_A, matrixA, n);
  fileToMatrix(path_matriz_B, matrixB, n);

  strassen(matrixA, matrixB, matrixC, n);

  if (output == 1)
  {
    matrixToFile("matrix/strassen_C.txt", matrixC, n);
  }
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0)
    printMatrix(matrixC, n);
  MPI_Finalize();
}

void fileToMatrix(char filename[], double arr[], int size)
{
  FILE *file = fopen(filename, "rb");
  fseek(file, 0, SEEK_SET);
  for (int i = 0; i < size; i++)
  {
    fseek(file, ((ulint)i * (ulint)size) * (ulint)sizeof(double), SEEK_SET);
    fread(&arr[i * size], sizeof(double), size, file);
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
  int root;

  if (size <= 2)
  {
    baseAlgorithm(a, b, c, size);
  }
  else
  {
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int newSize = size / 2;
    int numElements = newSize * newSize;
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

    double *t1 = (double *)malloc(mallocSize);
    double *t2 = (double *)malloc(mallocSize);
    double *t3 = (double *)malloc(mallocSize);
    double *t4 = (double *)malloc(mallocSize);
    double *t5 = (double *)malloc(mallocSize);
    double *t6 = (double *)malloc(mallocSize);
    double *t7 = (double *)malloc(mallocSize);
    double *t8 = (double *)malloc(mallocSize);
    double *t9 = (double *)malloc(mallocSize);
    double *t10 = (double *)malloc(mallocSize);

    double *p1 = (double *)malloc(mallocSize);
    double *p2 = (double *)malloc(mallocSize);
    double *p3 = (double *)malloc(mallocSize);
    double *p4 = (double *)malloc(mallocSize);
    double *p5 = (double *)malloc(mallocSize);
    double *p6 = (double *)malloc(mallocSize);
    double *p7 = (double *)malloc(mallocSize);

    double *cParc1 = (double *)malloc(mallocSize);
    double *cParc2 = (double *)malloc(mallocSize);
    double *cParc3 = (double *)malloc(mallocSize);
    double *cParc4 = (double *)malloc(mallocSize);

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

    double *tResults[10] = {t1, t2, t3, t4, t5, t6, t7, t8, t9, t10};
    double *pResults[7] = {p1, p2, p3, p4, p5, p6, p7};
    double *cResults[8] = {c12, c21, cParc1, cParc2, c11, cParc3, cParc4, c22};
    int workAddSub = 10;
    int workNum;
    int count = 0;
    for (int i = 0; i < workAddSub; i++)
    {
      workNum = count * worldSize + rank;
      if (workNum == i)
      {
        count++;
        switch (workNum)
        {
        case 0:
          sum(a11, a22, t1, newSize); // a11 + a22
          break;
        case 1:
          sum(b11, b22, t2, newSize); // b11 + b22
          break;
        case 2:
          sum(a21, a22, t3, newSize); // a21 + a22
          break;
        case 3:
          subtract(b12, b22, t4, newSize); // b12 - b22
          break;
        case 4:
          subtract(b21, b11, t5, newSize); // b21 - b11
          break;
        case 5:
          sum(a11, a12, t6, newSize); // a11 + a12
          break;
        case 6:
          subtract(a21, a11, t7, newSize); // a21 - a11
          break;
        case 7:
          sum(b11, b12, t8, newSize); // b11 + b12
          break;
        case 8:
          subtract(a12, a22, t9, newSize); // a12 - a22
          break;
        case 9:
          sum(b21, b22, t10, newSize); // b21 + b22
          break;
        }
      }
      root = i % worldSize;

      MPI_Bcast(tResults[i], numElements, MPI_DOUBLE, root, MPI_COMM_WORLD);

      // if (rank == 0)
      // {
      //   printMatrix(results[i], newSize);
      //   printf("********************\n");
      // }
    }

    int workMult = 7;
    count = 0;

    for (int i = 0; i < workMult; i++)
    {
      workNum = count * worldSize + rank;
      if (workNum)
      {
        switch (workNum)
        {
        case 0:
          strassen(t1, t2, p1, newSize); // p1 = (a11+a22) * (b11+b22)
          break;
        case 1:
          strassen(t3, b11, p2, newSize); // p2 = (a21+a22) * (b11)
          break;
        case 2:
          strassen(a11, t4, p3, newSize); // p3 = (a11) * (b12 - b22)
          break;
        case 3:
          strassen(a22, t5, p4, newSize); // p4 = (a22) * (b21 - b11)
          break;
        case 4:
          strassen(t6, b22, p5, newSize); // p5 = (a11+a12) * (b22)
          break;
        case 5:
          strassen(t7, t8, p6, newSize); // p6 = (a21-a11) * (b11+b12)
          break;
        case 6:
          strassen(t9, t10, p7, newSize); // p7 = (a12-a22) * (b21+b22)
          break;
        }
      }
      root = i % worldSize;
      MPI_Bcast(pResults[i], numElements, MPI_DOUBLE, root, MPI_COMM_WORLD);
    }

    int workC = 8;
    count = 0;
    for (int i = 0; i < workC; i++)
    {
      workNum = count * worldSize + rank;
      if (workNum == rank)
      {
        switch (workNum)
        {
        case 0:
          sum(p3, p5, c12, newSize); // c12 = p3 + p5
          break;
        case 1:
          sum(p2, p4, c21, newSize); // c21 = p2 + p4
          break;
        case 2:
          sum(p1, p4, cParc1, newSize); // p1 + p4
          break;
        case 3:
          sum(cParc1, p7, cParc2, newSize); // p1 + p4 + p7
          break;
        case 4:
          subtract(cParc2, p5, c11, newSize); // c11 = p1 + p4 - p5 + p7
          break;
        case 5:
          sum(p1, p3, cParc3, newSize); // p1 + p3
          break;
        case 6:
          sum(cParc3, p6, cParc4, newSize); // p1 + p3 + p6
          break;
        case 7:
          subtract(cParc4, p2, c22, newSize); // c22 = p1 + p3 - p2 + p6
          break;
        }
      }
      root = i % worldSize;
      MPI_Bcast(cResults[i], numElements, MPI_DOUBLE, root, MPI_COMM_WORLD);
    }

    if (rank == 0)
    {
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