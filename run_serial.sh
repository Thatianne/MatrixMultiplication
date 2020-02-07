#!/bin/bash
for n in 64 91 128 181 256 362 512 724 1024 1448 2048 2896 4096 5793 8192
do
    echo "Serial clássico tamanho $n"
    bin/serial_classic $n matrix/A matrix/B output/log.csv 0
    
    echo "Serial clássico OMP tamanho $n"
    bin/serial_classic_omp $n matrix/A matrix/B output/log.csv 0

    echo "SUMMA serial tamanho $n"
    bin/summa_serial $n matrix/A matrix/B output/log.csv 0
done
