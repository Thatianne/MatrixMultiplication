#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>
#include <sys/time.h>
#include "utils.c"

#define ALGORITMO "summa_mpi"
#define MASTER 0
#define ENVIO_OFFSET 1000
#define ENVIO_VALOR_INICIAL 2000

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Parametros invalidos, verifique...\n");
		return -1;
	}

	int n = atoi(argv[1]);
	char *logFile = argv[2];
	char *path_matriz = argv[3];
	FILE *fpA;
	FILE *fpB;
	fpA = fopen(path_matriz, "rb");

	//criação das matrizes
	 
	
	double *A;
	double *B;
	double *C;


	//---------------------------------------------------------------------------------
	// Configurações do MPI
	MPI_Init(&argc, &argv);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char mpi_processor_name[MPI_MAX_PROCESSOR_NAME];
	int mpi_name_len;
	MPI_Get_processor_name(mpi_processor_name, &mpi_name_len);

	MPI_Status status;

	int offset;
	int sobra;
	int inicio;
	if (mpi_world_rank == MASTER){
		offset = n/rank;
		sobra = n%rank;
		for(int dest=1; dest < world_size; i++){
			if(!(dest == world_size-1)){//caso não seja os ultimos computadores
				MPI_send(&offset, 1, MPI_INT, dest, ENVIO_OFFSET, MPI_COMM_WORLD);
				MPI_send(&(offset*i), 1, MPI_INT, dest, ENVIO_VALOR_INICIAL, MPI_COMM_WORLD);
			}
			else{ //caso seja o ultimo computador enviar também as sobras para ele computar
				MPI_send(&(offset+sobra), 1, MPI_INT, dest, ENVIO_OFFSET, MPI_COMM_WORLD);
				MPI_send(&(offset*i), 1, MPI_INT, dest, ENVIO_VALOR_INICIAL, MPI_COMM_WORLD);
			}
			
		}
	}
	else{
		int source = MASTER;
		MPI_Recv(&offset, 1, MPI_INT, source, ENVIO_OFFSET, MPI_COMM_WORLD, &status);
   		MPI_Recv(&inicio, 1, MPI_INT, source, ENVIO_VALOR_INICIAL, MPI_COMM_WORLD, &status);

		//tamanho da matriz c		LINHAS							COLUNAS						PROFUNDIDADE	
		unsigned long int tamanho2 = (unsigned long int)n * (unsigned long int)n * (unsigned long int)offset*(unsigned long int)sizeof(double);
		//tamanho das matrizes A e B	LINHA/COLUNAS		TAMANHO
		unsigned long int tamanho = (unsigned long int)n*(unsigned long int)sizeof(double);

		A = (double*) malloc(tamanho);
		B = (double*) malloc(tamanho);
		C = (double*) malloc(tamanho2);

		//faz para todo o offset
		for (int d =0; d<offset; d++){
			unsigned long int p1 = (unsigned long int)n*(unsigned long int)n*(unsigned long int)d;
			//lê a coluna da matriz A
			//nao é necessário liberar a matriz e alocar novamente pois sobrescrevemos os valores da matriz
			for (unsigned long int coluna = inicio; coluna< inicio+offset; coluna++){
				//salva a coluna da matriz A na variável 'A' 
				 fseek(fpA, 0, SEEK_SET);
				for(int i=0; i < n; i++){
					fseek (fp, i*tam*sizeof(double) + coluna*sizeof(double), SEEK_SET);
					fread(&A[i], sizeof(double), 1, fpA);
					fseek (fp, 0, SEEK_SET);
				}
				
				//Pega as linhas B
				for (unsigned long int linha = inicio; linha< inicio+offset; linha++){
					fseek(fpB, (unsigned long int)linha*(unsigned long int)n*(unsigned long int)sizeof(double), SEEK_SET);
					fread(B, sizeof(double),n, fpA);
					fseek(fpB, 0, SEEK_SET);

					//realiza a conta para cada coluna e cada linha
					for(int i=0; i< n; i++){
						unsigned long int p2 = (unsigned long int)i*(unsigned long int)n;
						for(int j=0; j< n; j++){
							C[p1+p2+j] = A[i]*C[j];
						}
					}

				}
			}
			
		}
	}

	MPI_Finalize();
	//---------------------------------------------------------------------------------

	return 0;
}
