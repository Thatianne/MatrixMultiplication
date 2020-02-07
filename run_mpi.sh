#!/bin/bash
for n in 64 91 128 181 256 362 512 724 1024 1448 2048 2896 4096 5793 8192
do
    for p in 5 7 9 10 11 12 13
    do
    echo "$n - Summa MPI p$p"
        mpirun -np $p --hostfile hostfile bin/summa_mpi $n matrix/A matrix/B output/log.csv

        echo "$n - STRASSEN MPI p$p"
        mpirun -np $p --hostfile hostfile bin/strassen_mpi $n matrix/A matrix/B output/log.csv

        echo "$n - Summa MPI OMP p$p"
            mpirun -np $p --hostfile hostfile bin/summa_mpi_omp $n matrix/A matrix/B output/log.csv 8

        echo "$n - STRASSEN MPI OMP p$p"
            mpirun -np $p --hostfile hostfile bin/strassen_mpi_omp $n matrix/A matrix/B output/log.csv 8
        done
    done
done