#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned long int ulint;

double randomDouble(double min, double max)
{
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}

double randomInt(int max)
{
	return rand() % max;
}

double *generateMatrix(ulint n)
{
	double *M = malloc(n * n * sizeof(double));

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			//M[i * n + j] = randomDouble(-999999999.0, 999999999.0);
			M[i * n + j] = randomInt(10);

	return M;
}

void printMatrix(double *M, char *label, ulint n)
{
	char binFile[100];
	sprintf(binFile, "matrix/%s", label);
	FILE *fpBin = fopen(binFile, "w+");
	char txtFile[100];
	sprintf(txtFile, "matrix/%s_%dx%d.txt", label, (int)n, (int)n);
	FILE *fpTxt = fopen(txtFile, "w+");

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			fwrite(&M[i * n + j], 1, sizeof(double), fpBin);
			fprintf(fpTxt, "%09.16lf ", M[i * n + j]);
		}
		fprintf(fpTxt, "\n");
	}

	fclose(fpBin);
	fclose(fpTxt);
}

int main(int argc, char *argv[])
{
	ulint n = (argc > 1) ? atoi(argv[1]) : 10;
	int seed = (argc > 2) ? atoi(argv[2]) : 0;

	srand(seed);

	double *A = generateMatrix(n);
	printMatrix(A, "A", n);

	double *B = generateMatrix(n);
	printMatrix(B, "B", n);

	return 0;
}
