#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.c"

void createDir(const char *path);
void print(const char *path_matriz, ulint n, const char *output);

void pre_process(int rank, ulint n, const char *path_matriz_A, const char *path_matriz_B)
{
	char outDir[100];
	createDir("./matrix/strassen/");
	sprintf(outDir, "./matrix/strassen/rank_%d", rank);
	createDir(outDir);
	sprintf(outDir, "./matrix/strassen/rank_%d/%ld", rank, n);
	createDir(outDir);

	char output[100];
	sprintf(output, "%s/A", outDir);
	print(path_matriz_A, n, output);
	sprintf(output, "%s/B", outDir);
	print(path_matriz_B, n, output);
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
	ulint n2Pow = to2Pow(n);
	ulint size = (ulint)(n2Pow / 2);
	for (int q = 1; q <= 4; q++)
	{
		char binFile[100];
		sprintf(binFile, "%s_q%d", output, q);
		FILE *fpBin = fopen(binFile, "w+");

		FILE *fpM = fopen(path_matriz, "rb");
		double M;
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
			for (ulint j = j_start; j < j_end; j++)
			{
				if (i >= n || j >= n)
				{
					M = 0.0;
				}
				else
				{
					fseek(fpM, 0, SEEK_SET);
					fseek(fpM, ((ulint)i * n + (ulint)j) * (ulint)sizeof(double), SEEK_SET);
					readed = fread(&M, sizeof(double), 1, fpM);
				}

				fwrite(&M, 1, sizeof(double), fpBin);
				fprintf(fpTxt, "%09.16lf ", M);
			}
			fprintf(fpTxt, "\n");
		}

		fclose(fpTxt);
		fclose(fpBin);
	}
}

double *read11(int rank, const char label, ulint n)
{
	char binFile[100];
	sprintf(binFile, "./matrix/strassen/rank_%d/%ld/%c_q1", rank, n, label);
	return readMatrix(binFile, n);
}

double *read12(int rank, const char label, ulint n)
{
	char binFile[100];
	sprintf(binFile, "./matrix/strassen/rank_%d/%ld/%c_q2", rank, n, label);
	return readMatrix(binFile, n);
}

double *read21(int rank, const char label, ulint n)
{
	char binFile[100];
	sprintf(binFile, "./matrix/strassen/rank_%d/%ld/%c_q3", rank, n, label);
	return readMatrix(binFile, n);
}

double *read22(int rank, const char label, ulint n)
{
	char binFile[100];
	sprintf(binFile, "./matrix/strassen/rank_%d/%ld/%c_q4", rank, n, label);
	return readMatrix(binFile, n);
}
