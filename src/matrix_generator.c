#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.c"

double randomDouble(double min, double max)
{
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}

double *generateMatrix(ulint n, int max)
{
	double *M = malloc(n * n * sizeof(double));

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (max > 0)
				M[i * n + j] = rand() % max;
			else
				M[i * n + j] = randomDouble(-999999999.0, 999999999.0);
		}
	}

	return M;
}

int main(int argc, char *argv[])
{
	ulint n = (argc > 1) ? atoi(argv[1]) : 10;
	int seed = (argc > 2) ? atoi(argv[2]) : 0;
	int max = (argc > 3) ? atof(argv[3]) : 0;

	srand(seed);

	double *A = generateMatrix(n, max);
	printMatrix("./matrix/A", A, n);
	printMatrixBin("./matrix/A.txt", A, n);

	double *B = generateMatrix(n, max);
	printMatrix("./matrix/B", B, n);
	printMatrixBin("./matrix/B.txt", B, n);

	return 0;
}
