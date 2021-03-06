#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

typedef unsigned long int ulint;

double getDiffTime(struct timeval t1, struct timeval t2)
{
	return (double)(t2.tv_usec - t1.tv_usec) / 1000000 + (double)(t2.tv_sec - t1.tv_sec);
}

void printLog(const char *filePath, const char *algoritmo, int n, double exec_time, double cpu_time, double read_time, double read_cpu_time)
{
	FILE *log;
	log = fopen(filePath, "a");
	fprintf(log, "%s,%d,%f,%f,%f,%f\n", algoritmo, n, exec_time, cpu_time, read_time, read_cpu_time);
	fclose(log);
}

void printLogMPI(const char *filePath, const char *algoritmo, int n, double exec_time, double user_time, const int rank, const int word_size)
{
	FILE *log;
	log = fopen(filePath, "a");
	fprintf(log, "%s,%d,%f,%f,%d, %d\n", algoritmo, n, exec_time, user_time, rank, word_size);
	fclose(log);
}

void printMatrix(const char *filePath, const double *M, int n)
{
	FILE *fp;
	fp = fopen(filePath, "w+");
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
			fprintf(fp, "%9.9lf ", M[i * n + j]);
		fprintf(fp, "\n");
	}
	fclose(fp);
}

void printMatrixBin(const char *filePath, const double *M, int n)
{
	FILE *fp;
	fp = fopen(filePath, "w+");
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
			fwrite(&M[i * n + j], 1, sizeof(double), fp);
	}
	fclose(fp);
}

void printMatrixBinColumn(const char *filePath, const double *M, int n)
{
	FILE *fp;
	fp = fopen(filePath, "w+");
	for (int j = 0; j < n; j++)
	{
		for (int i = 0; i < n; i++)
			fwrite(&M[i * n + j], 1, sizeof(double), fp);
	}
	fclose(fp);
}

int to2Pow(ulint n)
{
	int pow = 2;

	while (n > pow)
	{
		pow *= 2;
	}

	return pow;
}

double *readMatrix(const char *filePath, ulint n)
{
	double *M = (double *)malloc(n * n * (ulint)sizeof(double));
	FILE *fp = fopen(filePath, "rb");
	size_t readed;

	fseek(fp, 0, SEEK_SET);
	readed = fread(M, sizeof(double), n * n, fp);

	return M;
}
