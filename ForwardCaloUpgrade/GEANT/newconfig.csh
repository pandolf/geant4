#!/bin/csh

source /afs/cern.ch/sw/lcg/external/gcc/4.6.3/x86_64-slc6-gcc46-opt/setup.csh

setenv G4BASE /afs/cern.ch/sw/lcg/external/geant4

source $G4BASE/9.6.p02/x86_64-slc6-gcc46-opt/GNUmake-setup.csh


setenv ROOTSYS /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root
echo "  "
echo "You have choosen the next ROOTSYS version:"
echo "$ROOTSYS"

setenv HEPMC_DIR /afs/cern.ch/sw/lcg/external/HepMC/2.06.08/x86_64-slc6-gcc46-opt

echo " " 
echo "You have choosen the next HepMC version:" 
echo "$HEPMC_DIR"



if( $?LD_LIBRARY_PATH ) then
  setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:$HEPMC_DIR/lib
else
 echo "No LD_LIBRARY_PATH at all - check $LD_LIBRARY_PATH"
endif

if( $?LD_LIBRARY_PATH ) then
  setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:$ROOTSYS/lib
else
 echo "No LD_LIBRARY_PATH at all - check $LD_LIBRARY_PATH"
endif

setenv MyGeant ~/geant4/9.4.p02/x86_64-slc6-gcc46-opt/bin/Linux-g++

#echo " "
#echo "Your geant4 directory (alias MyGeant) with executable fcalor:"
#echo "$MyGeant"
#echo " "

source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/bin/thisroot.csh

setenv ROOTSYS /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root
setenv HEPMC_DIR /afs/cern.ch/sw/lcg/external/HepMC/2.06.08/x86_64-slc6-gcc46-opt
setenv MyGeant /afs/cern.ch/user/p/pandolf/geant4/9.4.p02/x86_64-slc6-gcc46-opt/bin/Linux-g++
setenv DYLD_LIBRARY_PATH /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:
setenv SHLIB_PATH /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:
setenv LIBPATH /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:
setenv PYTHONPATH /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib:
#setenv BASEDIR /afs/cern.ch/work/p/pandolf/geant4/
#setenv MANPATH /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/man:/afs/cern.ch/cms/share/man:/afs/cern.ch/cms/share/man:
#setenv LD_LIBRARY_PATH /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/lib




if( $?CMSSW_BASE ) then
  setenv BASEDIR $CMSSW_BASE/src
else
  setenv BASEDIR /afs/cern.ch/work/p/pandolf/geant4/
endif
