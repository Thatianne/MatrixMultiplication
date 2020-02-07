### Makefile ###

all:matrix_generator serial_classic serial_classic_omp summa_serial summa_mpi summa_mpi_omp strassen_serial #strassen_mpi strassen_mpi_omp

matrix_generator:
	gcc src/matrix_generator.c -o bin/matrix_generator

serial_classic:
	gcc -O3 src/serial_classic.c -o bin/serial_classic
serial_classic_omp:
	gcc -O3 src/serial_classic_omp.c -o bin/serial_classic_omp

summa_serial:
	gcc -O3 src/summa_serial.c -o bin/summa_serial
summa_mpi:
	mpicc -O3 src/summa_mpi.c -o bin/summa_mpi
summa_mpi_omp:
	mpicc -Wall -fopenmp -O3 -mavx2 summa_mpi_omp.c -fopt-info-vec -o bin/summa_mpi_omp

strassen_serial:
	gcc -O3 src/strassen_serial.c -o bin/strassen_serial
strassen_mpi:
	mpicc -O3 src/strassen_mpi.c -o bin/strassen_mpi
strassen_mpi_omp:
	mpicc -Wall -fopenmp -O3 strassen_mpi_omp.c -fopt-info-vec -o bin/strassen_mpi_omp