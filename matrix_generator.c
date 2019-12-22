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

void generateMatrix(char *label, ulint n)
{
	char binFile[100];
	sprintf(binFile, "matrix/%s", label, (int)n, (int)n);
	FILE *fpBin = fopen(binFile, "w+");

	char txtFile[100];
	sprintf(txtFile, "matrix/%s_%dx%d.txt", label, (int)n, (int)n);
	FILE *fpTxt = fopen(txtFile, "w+");

	double value;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			//value = randomDouble(-999999999.0, 999999999.0);
			value = randomInt(10);
			fwrite(&value, 1, sizeof(value), fpBin);
			fprintf(fpTxt, "%09.16lf ", value);
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

	generateMatrix("A", n);
	generateMatrix("B", n);

	return 0;
}
