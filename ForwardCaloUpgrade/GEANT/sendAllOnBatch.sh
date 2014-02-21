#!/bin/bash


if [ $# -ne 1 ] ; then
    echo "USAGE: ./sendAllOnBatch.sh batchDirName"
    exit
fi


for i in `seq 0 30`; do
    python sendOnBatch.py $1 $i 5 2 20
    python sendOnBatch.py $1 $i 5 5 20
    python sendOnBatch.py $1 $i 10 2 20
    python sendOnBatch.py $1 $i 10 5 20
done
