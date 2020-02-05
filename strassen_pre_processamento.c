#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned long int ulint;

ulint DIMENSOES[] = {64, 91, 128, 181, 256, 362, 512, 724, 1024, 1448, 2048, 2896, 4096, 5793, 8192};

void printMatrix(double *M, char *label, ulint n)
{
	ulint d;
	for (int k = 0; k < sizeof(DIMENSOES) / 4; k++)
	{
		d = DIMENSOES[k];
		if (d > n)
			break;

		char outDir[100];
		sprintf(outDir, "./matrix/strassen/%ld", d);
#if defined(_WIN32)
		_mkdir(outDir);
#else
		mkdir(outDir, 0700);
#endif

		for (int q = 1; q <= 4; q++)
		{
			char binFile[100];
			sprintf(binFile, "./matrix/%ld/%s_q%d", d, label, q);
			FILE *fpBin = fopen(binFile, "w+");

			char txtFile[100];
			sprintf(txtFile, "./matrix/%ld/%s_q%d.txt", d, label, q);
			FILE *fpTxt = fopen(txtFile, "w+");

			int _i = (q < 3) ? 0 : (d / 2);
			int _j = (q % 2 != 0) ? 0 : (d / 2);

			for (ulint i = _i, x = 0; x < d / 2; i++, x++)
			{
				for (ulint j = _j, y = 0; y < d / 2; j++, y++)
				{
					fwrite(&M[i * d + j], 1, sizeof(double), fpBin);
					fprintf(fpTxt, "%09.16lf ", M[i * d + j]);
				}
				fprintf(fpTxt, "\n");
			}

			fclose(fpBin);
			fclose(fpTxt);
		}
	}
}

int main(int argc, char *argv[])
{
#if defined(_WIN32)
	_mkdir("./matrix/strassen");
#else
	mkdir("./matrix/strassen", 0700);
#endif

	ulint n = (ulint)atoi(argv[1]);

	char *path_matriz_M = argv[2];
	double *M = (double *)malloc(n * n);
	FILE *fpM = fopen(path_matriz_M, "rb");
	size_t readed = fread(M, sizeof(double), n, fpM);
	printMatrix(M, "A", n);

	path_matriz_M = argv[3];
	M = (double *)malloc(n * n);
	fpM = fopen(path_matriz_M, "rb");
	readed = fread(M, sizeof(double), n, fpM);
	printMatrix(M, "B", n);

	return 0;
}
