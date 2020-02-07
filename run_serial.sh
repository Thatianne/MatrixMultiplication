#!/bin/bash
for n in 64 91 128
do
    echo "$n - Clássico"
    bin/classic $n matrix/A matrix/B output/log.csv

    echo "$n - Summa serial"
    bin/summa_serial $n matrix/A matrix/B output/log.csv

    echo " "
done
