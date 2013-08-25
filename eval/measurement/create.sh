#!/bin/sh

for i in `seq 0 5`
do
    for j in `seq 0 3`
    do
        echo "python ./file_cdf.py ${i} ${j}"
        eval "python ./file_cdf.py ${i} ${j}"
    done
done
