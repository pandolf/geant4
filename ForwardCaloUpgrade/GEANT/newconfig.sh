#!/bin/bash

source /afs/cern.ch/sw/lcg/external/gcc/4.6.3/x86_64-slc6-gcc46-opt/setup.sh

export G4BASE=/afs/cern.ch/sw/lcg/external/geant4

source $G4BASE/9.6.p02/x86_64-slc6-gcc46-opt/GNUmake-setup.sh


export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root
echo "  "
echo "You have choosen the next ROOTSYS version:"
echo "$ROOTSYS"

export HEPMC_DIR=/afs/cern.ch/sw/lcg/external/HepMC/2.06.08/x86_64-slc6-gcc46-opt

echo " " 
echo "You have choosen the next HepMC version:" 
echo "$HEPMC_DIR"



if [ $?LD_LIBRARY_PATH ] ; then
  export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$HEPMC_DIR/lib
else
  echo "No LD_LIBRARY_PATH at all - check $LD_LIBRARY_PATH"
fi

if [ $?LD_LIBRARY_PATH ] ; then
  export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$ROOTSYS/lib
else
 echo "No LD_LIBRARY_PATH at all - check $LD_LIBRARY_PATH"
fi

export MyGeant=~/geant4/9.4.p02/x86_64-slc6-gcc46-opt/bin/Linux-g++

echo " "
echo "Your geant4 directory (alias MyGeant) with executable fcalor:"
echo "$MyGeant"
echo " "

source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/bin/thisroot.sh

export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root
export HEPMC_DIR=/afs/cern.ch/sw/lcg/external/HepMC/2.06.08/x86_64-slc6-gcc46-opt
export MyGeant=/afs/cern.ch/user/p/pandolf/geant4/9.4.p02/x86_64-slc6-gcc46-opt/bin/Linux-g++
export DYLD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:
export SHLIB_PATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:
export LIBPATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:
export PYTHONPATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:
export BASEDIR=/afs/cern.ch/work/p/pandolf/geant4/



