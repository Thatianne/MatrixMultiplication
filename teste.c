#pragma GCC optimize("O3")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int* gerarMatriz(int);
int main(){
    FILE *  fp = fopen("/home/gabriel/50.txt", "w+");
    int tam = 10;
    int* matriz = gerarMatriz(tam);
    int* A = matriz;
    for(int i=0; i < tam; i++){
        for (int j=0; j<tam; j++){
            printf("\t%d", A[i*tam + j]);
        }
        printf("\n");
    }

    fwrite(matriz, tam*tam, sizeof(int), fp);
    fclose(fp);

    fp = fopen("/home/gabriel/50.txt", "rb");
    
    int* linha =(int *) malloc(tam*sizeof(int));
    int* coluna = (int *) malloc(tam*sizeof(int));
    
    //estrtura para pular
    fseek (fp, 3*tam*sizeof(int), SEEK_SET);
    fread(linha, sizeof(int), tam, fp);
    printf("\n\n");
    for (int j=0; j<tam; j++){
            printf("\t%d", linha[j]);
        }
    printf("\n");

    /*for(int i=0; i < tam; i++){
        for (int j=0; j<tam; j++){
            fseek (fp, i*tam*sizeof(int) + j, SEEK_SET);
            fread(&coluna[j], sizeof(int), 1, fp);
        }
    }*/
    fseek (fp, 0, SEEK_SET);
    for(int i=0; i < tam; i++){
            fseek (fp, i*tam*sizeof(int) + 3*sizeof(int), SEEK_SET);
            fread(&coluna[i], sizeof(int), 1, fp);
            fseek (fp, 0, SEEK_SET);
            printf("\t%d", coluna[i]);
        }
    
    printf("\n");
}

int* gerarMatriz(int tam){
    int *A = (int *) malloc(tam*tam*sizeof(int));

    int k = 0;
    for(int i=0; i < tam; i++){
        for (int j=0; j<tam; j++){
            A[i*tam + j] = k++;
        }
    }

    return A;
    
}
