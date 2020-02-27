#!/bin/bash
for t in 1 2 3
do
    for n in 64 91 128 181 256 362 512 724 1024 1448 2048 2896 4096 5793 8192 11585 16384
    do
        echo "$n - Clássico Otimizado"
        bin/classic_otimizado $n matrix/A_row matrix/B_column output/log.csv

        echo "$n - Summa serial"
        bin/summa_serial $n matrix/A_column matrix/B_row output/log.csv
    done
done
