#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double randomDouble(double min, double max)
{
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}

int main(int argc, char *argv[])
{
	unsigned long int n = (argc > 1) ? atoi(argv[1]) : 10;
	int seed = (argc > 2) ? atoi(argv[2]) : 0;

	srand(seed);

	char fileName[100];
	sprintf(fileName, "matrix/%dx%d_%d.txt", (int)n, (int)n, seed);
	FILE *fp;
	fp = fopen(fileName, "w+");

	unsigned long int x;
	double value;

	for (x = 0; x < (n * n); x++)
	{
		value = randomDouble(-999.0, 999.0);
		fwrite(&value, 1, sizeof(value), fp);
	}

	fclose(fp);

	return 0;
}
