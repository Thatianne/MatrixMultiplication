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
			M[i * n + j] = randomInt(30);

	return M;
}

void printMatrix(double *M, char *label, ulint n)
{
	char binFile[100];
	sprintf(binFile, "matrix/%s", label);
	FILE *fpBin = fopen(binFile, "w+");
	char txtFile[100];
	sprintf(txtFile, "matrix/%s.txt", label);
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

void printMatrixQ(double *M, char *label, ulint n)
{
	for (int q = 1; q <= 4; q++)
	{
		char binFile[100];
		sprintf(binFile, "matrix/%s_q%d", label, q);
		FILE *fpBin = fopen(binFile, "w+");

		char txtFile[100];
		sprintf(txtFile, "matrix/%s_q%d.txt", label, q);
		FILE *fpTxt = fopen(txtFile, "w+");

		int _i = (q < 3) ? 0 : (n / 2);
		int _j = (q % 2 != 0) ? 0 : (n / 2);

		for (int i = _i, x = 0; x < n / 2; i++, x++)
		{
			for (int j = _j, y = 0; y < n / 2; j++, y++)
			{
				fwrite(&M[i * n + j], 1, sizeof(double), fpBin);
				fprintf(fpTxt, "%09.16lf ", M[i * n + j]);
			}
			fprintf(fpTxt, "\n");
		}

		fclose(fpBin);
		fclose(fpTxt);
	}
}

int main(int argc, char *argv[])
{
	ulint n = (argc > 1) ? atoi(argv[1]) : 10;
	int seed = (argc > 2) ? atoi(argv[2]) : 0;

	srand(seed);

	double *A = generateMatrix(n);
	printMatrix(A, "A", n);
	printMatrixQ(A, "A", n);

	double *B = generateMatrix(n);
	printMatrix(B, "B", n);
	printMatrixQ(B, "B", n);

	return 0;
}
