#!/bin/bash
for n in 64 91 128 181 256 362 512 724 1024 1448 2048 2896 4096 5793 8192
do
    for p in 5 7 9 10 11 12 13
    do
        echo "SUMMA MPI tamanho $n NumProc $p"
        mpirun -np $p --hostfile hostfile bin/summa_mpi $n matrix/A matrix/B 0

        echo "STRASSEN MPI tamanho $n NumProc $p"
        #mpirun -np $p --hostfile hostfile bin/strassen_mpi $n matrix/A matrix/B 0

        for t in $(seq 1 8)
        do
            echo "SUMMA MPI OpenMP tamanho $n NumProc $p NumThreads $t"
            mpirun -np $p --hostfile hostfile bin/summa_mpi_omp $t $n matrix/A matrix/B 0

            echo "STRASSEN MPI OpenMP tamanho $n NumProc $p NumThreads $t"
            #mpirun -np $p --hostfile hostfile bin/strassen_mpi_omp $t $n matrix/A matrix/B 0
        done
    done
done
