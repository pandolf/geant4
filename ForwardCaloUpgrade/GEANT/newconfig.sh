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

export PATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/bin:/afs/cern.ch/user/p/pandolf/geant4_workdir/bin/Linux-g++:/afs/cern.ch/sw/lcg/contrib/gcc/4.6.3/x86_64-slc6/bin:/afs/cern.ch/cms/bin/amd64_linux26:/afs/cern.ch/cms/system/bin:/afs/cern.ch/user/p/pandolf/scripts:/afs/cern.ch/cms/caf/scripts:/afs/cern.ch/cms/common:/afs/cern.ch/cms/bin:/afs/cern.ch/cms/utils:/usr/sue/bin:/usr/lib64/qt-3.3/bin:/usr/kerberos/sbin:/usr/kerberos/bin:/usr/local/bin:/bin:/usr/bin:/afs/cern.ch/project/eos/installation/pro/bin
export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root
export HEPMC_DIR=/afs/cern.ch/sw/lcg/external/HepMC/2.06.08/x86_64-slc6-gcc46-opt
export MyGeant=/afs/cern.ch/user/p/pandolf/geant4/9.4.p02/x86_64-slc6-gcc46-opt/bin/Linux-g++
export DYLD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:
export SHLIB_PATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:
export LIBPATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:
export PYTHONPATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:
export LD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:/afs/cern.ch/sw/lcg/external/clhep/2.1.3.1/x86_64-slc6-gcc46-opt/lib:/afs/cern.ch/sw/lcg/external/geant4/9.6.p02/x86_64-slc6-gcc46-opt/lib64:/afs/cern.ch/sw/lcg/contrib/gcc/4.6.3/x86_64-slc6/lib64:/afs/cern.ch/sw/lcg/contrib/mpfr/2.4.2/x86_64-slc6/lib:/afs/cern.ch/sw/lcg/contrib/gmp/4.3.2/x86_64-slc6/lib:/afs/cern.ch/sw/lcg/external/XercesC/3.1.1/x86_64-slc6-gcc46-opt/lib:/afs/cern.ch/sw/lcg/external/qt/4.6.3p2/x86_64-slc6-gcc45-opt/lib:/afs/cern.ch/sw/lcg/external/HepMC/2.06.08/x86_64-slc6-gcc46-opt/lib
export BASEDIR=/afs/cern.ch/work/p/pandolf/geant4/



