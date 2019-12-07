#!/bin/bash

for a in serial_classic strassen_serial strassen_mpi summa_serial summa_mpi
do
    echo "executando $a..."
    for i in 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536
    do
        echo "tamanho $i"
        ./bin/$a.bin $i output/log.csv
    done
done

for a in strassen_mpi_omp summa_mpi_omp
do
    echo "executando $a..."
    for i in 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536
    do
        echo "tamanho $i"
        for t in 1 2 3 4 5 6 7 8 9 10 11 12
        do
            ./bin/$a.bin $i output/log.csv $t
        done
    done
done