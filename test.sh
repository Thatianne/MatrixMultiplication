#!/bin/bash
./bin/matrix_generator 16 50
echo "Classic"
bin/classic_otimizado 16 matrix/A_row matrix/B_column output/log.csv 1
echo "Summa"
bin/summa_serial 16 matrix/A_column matrix/B_row output/log.csv 1
echo "Summa MPI"
mpirun -np 1 --hostfile hostfile bin/summa_mpi 16 matrix/A_column matrix/B_row output/log.csv 1
echo "Strassen MPI"
mpirun -np 1 --hostfile hostfile bin/strassen_mpi 16 matrix/A_row matrix/B_row output/log.csv 1
