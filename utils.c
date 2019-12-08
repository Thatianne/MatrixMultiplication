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

void printMatrix(FILE *fp, int size, int *M)
{
    int m, n;
    for (m = 0; m < size; m++)
    {
        for (n = 0; n < size; n++)
            fprintf(fp, "%d ", *(M + size * m + n));
        fprintf(fp, "\n");
    }
}

int *createMatrix(int size, int subseed)
{
    int *A = malloc(size * size * sizeof(int));
    int m, n, *x = A;
    srand(587938712 + subseed);
    for (m = 0; m < size; m++)
        for (n = 0; n < size; n++)
            *(x++) = rand() % 100;
    return A;
}

void writeOutput(char *algoritmo, int size, int *A, int *B, int *C)
{
    unsigned long time = curtime() * 1000;

    char fileName[100];
    sprintf(fileName, "output/%s_%ld.txt", algoritmo, time);

    FILE *fp;
    fp = fopen(fileName, "w+");

    printMatrix(fp, size, A);
    fprintf(fp, "\n");
    printMatrix(fp, size, B);
    fprintf(fp, "\n");
    printMatrix(fp, size, C);

    fclose(fp);
}

void writeLog(char *fileName, char *algoritmo, int size, double allTime, double execTime)
{
    FILE *log;
    log = fopen(fileName, "a");
    fprintf(log, "%s,%d,%f,%f\n", algoritmo, size, allTime, execTime);
    fclose(log);
}