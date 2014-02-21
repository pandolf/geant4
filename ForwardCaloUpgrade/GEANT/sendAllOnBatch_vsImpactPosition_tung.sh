#!/bin/bash


if [ $# -ne 1 ] ; then
    echo "USAGE: ./sendAllOnBatch_tung.sh batchDirName"
    exit
fi

cat scripts/simulation_750MeV_batch.sh | sed 's/absorber=(Lead)/absorber=(Tungsten)/' > scripts/simulation_750MeV_tung_batch.sh
cat sendOnBatch.py | sed 's/simulation_750MeV_batch/simulation_750MeV_tung_batch/' > sendOnBatch_tung.py


for i in `seq 0 45`; do
    python sendOnBatch_tung.py $1 15 10 3 25 $i
    python sendOnBatch_tung.py $1 15  5 3 25 $i
done
