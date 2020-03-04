#!/bin/bash
echo "./bin/matrix_generator 8 50"
./bin/matrix_generator 8 10
echo "Classic"
echo "bin/classic_otimizado 8 matrix/A_row matrix/B_column output/log.csv 1"
bin/classic_otimizado 8 matrix/A_row matrix/B_column output/log.csv 1
echo "Summa"
echo "bin/summa_serial 8 matrix/A_column matrix/B_row output/log.csv 1"
bin/summa_serial 8 matrix/A_column matrix/B_row output/log.csv 1
echo "Summa MPI"
echo "mpirun -np 1 --hostfile hostfile bin/summa_mpi 8 matrix/A_column matrix/B_row output/log.csv 1"
mpirun -np 1 --hostfile hostfile bin/summa_mpi 8 matrix/A_column matrix/B_row output/log.csv 1
echo "Strassen MPI"
echo "mpirun -np 1 --hostfile hostfile bin/strassen_mpi 8 matrix/A_row matrix/B_row output/log.csv 1"
mpirun -np 8 --mca orte_base_help_aggregate 0 --hostfile hostfile bin/strassen_mpi 8 matrix/A_row matrix/B_row output/log.csv 1