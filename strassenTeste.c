#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mpi.h"

void printMatrix(double arr[], int size);

int main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  int worldSize;
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Request req;
  MPI_Status status;

  double elements[8];
  int numElements = 8;

  int front = 0;
  int sender = 1;
  int i;
  if (rank == sender)
  {

    for (i = 0; i < 8; i++)
    {
      elements[i] = (double)i * 3.14;
    }
    MPI_Isend(&elements, numElements, MPI_DOUBLE, front, 123, MPI_COMM_WORLD, &req);
  }
  else if (rank == front)
  {
    MPI_Irecv(&elements, numElements, MPI_DOUBLE, sender, 123, MPI_COMM_WORLD, &req);
  }

  if (rank == front)
  {
    MPI_Wait(&req, &status);
    printMatrix(elements, numElements);
  }

  MPI_Finalize();
}

void printMatrix(double arr[], int size)
{
  for (int i = 0; i < size; i++)
  {
    printf("%f\t", arr[i]);
  }
  printf("\n");
}