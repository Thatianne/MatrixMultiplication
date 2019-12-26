#!/bin/bash
for i in 100 150 200 300 400 600 800 1200 1600 2400 3200 4800 6400 9600 12800 19200 25600 38400 51200 76800 102400 153600 204800 307200 409600 614400 819200 1228800 1638400 2457600 3276800 4915200 6553600 9830400 13107200 19660800 26214400 39321600 52428800 78643200 104857600 157286400 209715200 314572800 419430400
do
    echo "Serial clássico tamanho $i"
    bin/serial_classic $i matrix/A matrix/B 0

    echo "SUMMA serial tamanho $i"
    bin/summa_serial $i matrix/A matrix/B 0

    echo "STRASSEN serial tamanho $i"
    #bin/strassen_serial $i matrix/A matrix/B 0

    for p in $(seq 1 13)
    do
        echo "SUMMA MPI tamanho $i NumProc $p"
        mpirun -np $p --hostfile hostfile bin/summa_mpi $i matrix/A matrix/B 0

        echo "STRASSEN MPI tamanho $i NumProc $p"
        #mpirun -np $p --hostfile hostfile bin/strassen_mpi $i matrix/A matrix/B 0

        for t in $(seq 1 8)
        do
            echo "SUMMA MPI OpenMP tamanho $i NumProc $p NumThreads $t"
            mpirun -np $p --hostfile hostfile bin/summa_mpi_omp $t $i matrix/A matrix/B 0

            echo "STRASSEN MPI OpenMP tamanho $i NumProc $p NumThreads $t"
            #mpirun -np $p --hostfile hostfile bin/strassen_mpi_omp $t $i matrix/A matrix/B 0
        done
    done
done
