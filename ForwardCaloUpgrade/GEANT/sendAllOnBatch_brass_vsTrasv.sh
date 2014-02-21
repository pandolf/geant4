#!/bin/bash


if [ $# -ne 1 ] ; then
    echo "USAGE: ./sendAllOnBatch_brass.sh batchDirName"
    exit
fi

cat scripts/simulation_750MeV_batch.sh | sed 's/absorber=(Lead)/absorber=(Brass_def)/' > scripts/simulation_750MeV_brass_batch.sh
cat sendOnBatch.py | sed 's/simulation_750MeV_batch/simulation_750MeV_brass_batch/' > sendOnBatch_brass.py

for i in `seq 0 40`; do
    python sendOnBatch_brass.py $1 15 10 5 $i
done

