#!/bin/bash

for a in serial_classic summa_serial summa_mpi #summa_mpi_omp strassen_mpi strassen_mpi_omp
do
    echo "executando $a..."
    for i in 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536
    do
        echo "tamanho $i"
        ./bin/$a $i output/log.csv
    done
done