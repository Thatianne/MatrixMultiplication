### Makefile ###

all:matrix_generator serial_classic strassen_mpi strassen_mpi_omp summa_serial summa_mpi summa_mpi_omp

matrix_generator:
	gcc matrix_generator.c -o bin/matrix_generator

serial_classic:
	gcc -O3 serial_classic.c -o bin/serial_classic

strassen_mpi:
	mpicc -O3 strassen_mpi.c -o bin/strassen_mpi

strassen_mpi_omp:
	mpicc -Wall -fopenmp -O3 strassen_mpi_omp.c -o bin/strassen_mpi_omp

summa_serial:
	gcc -O3 summa_serial.c -o bin/summa_serial

summa_mpi:
	mpicc -O3 summa_mpi.c -o bin/summa_mpi

summa_mpi_omp:
	mpicc -Wall -fopenmp -O3 summa_mpi_omp.c -o bin/summa_mpi_omp