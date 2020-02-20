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

	double v;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (max > 0)
				M[i * n + j] = rand() % max;
			else
			{
				v = randomDouble(-999999999.0, 999999999.0);
				M[i * n + j] = v;
				// Se v é zero, um, ou um inteiro, volta o j para gerá-lo novamente
				if (((int)v) == 0 || ((int)v) == 1 || ((int)v) == v)
					j--;
			}
		}
	}

	return M;
}

int main(int argc, char *argv[])
{
	ulint n = (argc > 1) ? atoi(argv[1]) : 10;
	int max = (argc > 2) ? atof(argv[2]) : 0;
	int seed = (argc > 3) ? atoi(argv[3]) : 0;

	srand(seed);

	double *A = generateMatrix(n, max);
	printMatrix("./matrix/A.txt", A, n);
	printMatrixBin("./matrix/A", A, n);

	double *B = generateMatrix(n, max);
	printMatrix("./matrix/B.txt", B, n);
	printMatrixBin("./matrix/B", B, n);

	return 0;
}
