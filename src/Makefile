### Makefile ###

all:matrix_generator classic classic_otimizado summa_serial summa_mpi summa_mpi_omp #strassen_pre_processamento strassen_serial #strassen_mpi strassen_mpi_omp

matrix_generator:
	gcc -std=c99 src/matrix_generator.c -o bin/matrix_generator

classic:
	gcc -std=c99 -O3 src/classic.c -o bin/classic
classic_otimizado:
	gcc -std=c99 -O3 src/classic_otimizado.c -o bin/classic_otimizado

summa_serial:
	gcc -std=c99 -O3 src/summa_serial.c -o bin/summa_serial
summa_mpi:
	mpicc -std=c99 -O3 src/summa_mpi.c -o bin/summa_mpi
summa_mpi_omp:
	mpicc -std=c99 -Wall -fopenmp  -O3 src/summa_mpi_omp.c -o bin/summa_mpi_omp

#strassen_pre_processamento:
#	gcc -O3 src/strassen_pre_processamento.c -o bin/strassen_pre_processamento
#strassen_serial:
#	gcc -O3 src/strassen_serial.c -o bin/strassen_serial
#strassen_mpi:
#	mpicc -O3 src/strassen_mpi.c -o bin/strassen_mpi
#strassen_mpi_omp:
#	mpicc -Wall -fopenmp -fopt-info-vec -mavx2 -O3 src/strassen_mpi_omp.c -o bin/strassen_mpi_omp
