#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.c"

ulint DIMENSOES[] = {64, 91, 128, 181, 256, 362, 512, 724, 1024, 1448, 2048, 2896, 4096, 5793, 8192};

void createDir(const char *path);
void print(const char *path_matriz, ulint n, const char *output);

void execute(const char *path_matriz_A, const char *path_matriz_B, ulint max)
{
	createDir("./matrix/strassen");

	ulint n;
	for (int x = 0; x < sizeof(DIMENSOES) / 4; x++)
	{
		n = DIMENSOES[x];
		if (n > max)
			break;

		char outDir[100];
		sprintf(outDir, "./matrix/strassen/%ld", n);
		createDir(outDir);

		char output[100];
		sprintf(output, "%s/A", outDir);
		print(path_matriz_A, n, output);
		sprintf(output, "%s/B", outDir);
		print(path_matriz_B, n, output);
	}
}

void createDir(const char *path)
{
#if defined(_WIN32)
	_mkdir(path);
#else
	mkdir(path, 0700);
#endif
}

void print(const char *path_matriz, ulint n, const char *output)
{
	ulint size = (n / 2);
	for (int q = 1; q <= 4; q++)
	{
		char binFile[100];
		sprintf(binFile, "%s_q%d", output, q);
		FILE *fpBin = fopen(binFile, "w+");

		FILE *fpM = fopen(path_matriz, "rb");
		double *M = (double *)malloc(size);
		size_t readed;

		char txtFile[100];
		sprintf(txtFile, "%s_q%d.txt", output, q);
		FILE *fpTxt = fopen(txtFile, "w+");

		ulint i_start = (ulint)(q == 1 || q == 2) ? 0 : size;
		ulint i_end = i_start + size;
		ulint j_start = (q == 1 || q == 3) ? 0 : size;
		ulint j_end = j_start + size;

		for (ulint i = i_start; i < i_end; i++)
		{
			fseek(fpM, 0, SEEK_SET);
			fseek(fpM, ((ulint)i * n + j_start) * (ulint)sizeof(double), SEEK_SET);
			printf("%s %d\n", path_matriz, size);
			readed = fread(M, sizeof(double), (int) size, fpM);

			//fwrite(&M, size, sizeof(double), fpBin);

			for (ulint x = 0; x < size; x++)
			{
				//fprintf(fpTxt, "%09.16lf ", M[x]);
			}
			fprintf(fpTxt, "\n");
			printf("TESTE");
		}

		fclose(fpTxt);
		fclose(fpBin);
	}
}

int main(int argc, char *argv[])
{
	ulint max = (ulint)atoi(argv[1]);
	char *path_matriz_A = argv[2];
	char *path_matriz_B = argv[3];
	execute(path_matriz_A, path_matriz_B, max);
	return 0;
}
