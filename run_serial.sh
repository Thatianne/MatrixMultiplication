#!/bin/bash
for n in 64 91 128 181 256 362 512 724 1024 1448 2048 2896 4096 5793 8192
do
    echo "$n - Summa serial"
    bin/summa_serial $n matrix/A matrix/B output/log.csv
done
for n in 64 91 128 181 256 362 512 724 1024 1448 2048 2896 4096 5793 8192
do
    echo "$n - Clássico"
    bin/classic $n matrix/A matrix/B output/log.csv
done
