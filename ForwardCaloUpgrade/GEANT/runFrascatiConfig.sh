cp scripts/simulation_750MeV_batch.sh scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/simulation_750MeV_batch/s/simulation_750MeV_batch/simulation_750MeV_local/g" scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/events=2500/s/events=2500/events=10000/g" scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/rootfiles\/samplinghistos/s/rootfiles\/samplinghistos/rootfiles\/frasc_samplinghistos/g" scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/tables\/parameters/s/tables\/parameters/tables\/frasc_parameters/g" scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/rootfiles\/temp\/temp_/s/rootfiles\/temp\/temp_/rootfiles\/temp\/frasctemp_/g" scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/rootfile_sed=/s/temp_/frasctemp_/g" scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/absorber=/s/Lead/Tungsten/g" scripts/simulation_750MeV_local_tmp.sh
sed -i'' "/energ=/s/75/50/g" scripts/simulation_750MeV_local_tmp.sh
chmod +x ./scripts/simulation_750MeV_local_tmp.sh

./scripts/simulation_750MeV_local_tmp.sh  full 10 10 3 25

mkdir -p batchOutput_frascati_500MeV_v1/rootfiles/temp

mv rootfiles/frasc_samplinghistos_n10_act10_abs3_trasv25.root ./batchOutput_frascati_500MeV_v1/rootfiles
mv rootfiles/temp/frasctemp_n10_act10_abs3_trasv25.root ./batchOutput_frascati_500MeV_v1/rootfiles/temp/
