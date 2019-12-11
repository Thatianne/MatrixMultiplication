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

void printMatrix(FILE *fp, int n, int *M)
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
            fprintf(fp, "%d ", M[n * i + j]);
        fprintf(fp, "\n");
    }
}

/**
 * Gera uma matriz de números aleatórios entre -1000 a 1000
 * de dimensões d x d.
 */
int *createMatrix(int n, int subseed)
{
    int *M = (int *)malloc(n * n * sizeof(int));
    int i, j, *x = M;
    srand(587938712 + subseed);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            *(x++) = (rand() % 2000) - 1000;
    return M;
}

void writeOutput(char *algoritmo, int n, int *A, int *B, int *C)
{
    unsigned long time = curtime() * 1000;

    char fileName[100];
    sprintf(fileName, "output/%s_%ld.txt", algoritmo, time);

    FILE *fp;
    fp = fopen(fileName, "w+");

    printMatrix(fp, n, A);
    fprintf(fp, "\n");
    printMatrix(fp, n, B);
    fprintf(fp, "\n");
    printMatrix(fp, n, C);

    fclose(fp);
}

void writeLog(char *fileName, char *algoritmo, int n, double allTime, double execTime)
{
    FILE *log;
    log = fopen(fileName, "a");
    fprintf(log, "%s,%d,%f,%f\n", algoritmo, n, allTime, execTime);
    fclose(log);
}