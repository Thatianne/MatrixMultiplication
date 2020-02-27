#!/bin/bash
for n in 64 91 128 181 256 362 512 724 1024 1448 2048 2896 4096 5793 8192 11585 16384
do
    for p in 5 7 9 10 11 12 13
    do
    echo "$n - Summa MPI p$p"
        mpirun -np $p --hostfile hostfile bin/summa_mpi $n matrix/A_column matrix/B_row output/log.csv

        echo "$n - Strassen MPI p$p"
        mpirun -np $p --hostfile hostfile bin/strassen_mpi $n matrix/A_row matrix/B_row output/log.csv

        echo "$n - Summa MPI OMP p$p"
            mpirun -np $p --hostfile hostfile bin/summa_mpi_omp $n matrix/A_column matrix/B_row output/log.csv 8

        echo "$n - Strassen MPI OMP p$p"
            mpirun -np $p --hostfile hostfile bin/strassen_mpi_omp $n matrix/A_row matrix/B_row output/log.csv 8
        done
    done
done
