#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

struct timeval tv;
static double curtime()
{
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void printMatrix(FILE *fp, int dimension, int *M)
{
    int i, j;
    for (i = 0; i < dimension; i++)
    {
        for (j = 0; j < dimension; j++)
            fprintf(fp, "%d ", *(M + dimension * i + j));
        fprintf(fp, "\n");
    }
}

/**
 * Gera uma matriz de números aleatórios entre -1000 a 1000
 * de dimensões d x d.
 */
int *createMatrix(int dimension, int subseed)
{
    int *M = malloc(dimension * dimension * sizeof(int));
    int i, j, *x = M;
    srand(587938712 + subseed);
    for (i = 0; i < dimension; i++)
        for (j = 0; j < dimension; j++)
            *(x++) = (rand() % 2000) - 1000;
    return M;
}

void writeOutput(char *algoritmo, int dimension, int *A, int *B, int *C)
{
    unsigned long time = curtime() * 1000;

    char fileName[100];
    sprintf(fileName, "output/%s_%ld.txt", algoritmo, time);

    FILE *fp;
    fp = fopen(fileName, "w+");

    printMatrix(fp, dimension, A);
    fprintf(fp, "\n");
    printMatrix(fp, dimension, B);
    fprintf(fp, "\n");
    printMatrix(fp, dimension, C);

    fclose(fp);
}

void writeLog(char *fileName, char *algoritmo, int dimension, double allTime, double execTime)
{
    FILE *log;
    log = fopen(fileName, "a");
    fprintf(log, "%s,%d,%f,%f\n", algoritmo, dimension, allTime, execTime);
    fclose(log);
}