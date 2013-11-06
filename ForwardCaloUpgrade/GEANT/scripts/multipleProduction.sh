#! /bin/sh

source /afs/cern.ch/sw/lcg/external/gcc/4.3.2/x86_64-slc5/setup.sh
export G4BASE=/afs/cern.ch/sw/lcg/external/geant4
source $G4BASE/9.4.p02/x86_64-slc5-gcc43/setup.sh

i=1

while [ $i -le 7 ]; do

case $i in
1) ENERGY=10;;
2) ENERGY=20;;
3) ENERGY=40;;
4) ENERGY=80;;
5) ENERGY=160;;
6) ENERGY=320;;
7) ENERGY=640;;
esac

case $i in
1) NEVENTS=700;;
2) NEVENTS=700;;
3) NEVENTS=500;;
4) NEVENTS=400;;
5) NEVENTS=300;;
6) NEVENTS=300;;
7) NEVENTS=200;;
esac


sed s/'#ENERGY'/${ENERGY}/ <STEERING_CARDS/ecal_PBWO_HOMOGENEOUS.in | 
sed s/'#EVENTS'/${NEVENTS}/ >ecal_PBWO_HOMOGENEOUS_launch.in

~/geant4/9.4.p02/x86_64-slc5-gcc43/bin/Linux-g++/fcalor ecal_PBWO_HOMOGENEOUS_launch.in
cp test_01.root OUT/PBWO_FEB2012_FULLSIM_PERFECT_LIGHT_COLLECTION_ELECTRON_${ENERGY}.root


i=$[i+1]
done


	
