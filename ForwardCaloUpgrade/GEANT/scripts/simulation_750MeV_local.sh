#!/bin/bash


cp scripts/simulation_750MeV_batch.sh scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/simulation_750MeV_batch/s/simulation_750MeV_batch/simulation_750MeV_local/g" scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/events=2500/s/events=2500/events=100/g" scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/rootfiles\/samplinghistos/s/rootfiles\/samplinghistos/rootfiles\/prova_samplinghistos/g" scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/tables\/parameters/s/tables\/parameters/tables\/prova_parameters/g" scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/rootfiles\/temp\/temp_/s/rootfiles\/temp\/temp_/rootfiles\/temp\/provatemp_/g" scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/rootfile_sed=/s/temp_/provatemp_/g" scripts/simulation_750MeV_local_tmp.sh
chmod +x ./scripts/simulation_750MeV_local_tmp.sh

./scripts/simulation_750MeV_local_tmp.sh $*
#rm ./scripts/simulation_750MeV_local_tmp.sh
