#!/bin/csh

 source /afs/cern.ch/sw/lcg/external/gcc/4.3.2/x86_64-slc5/setup.csh

 setenv G4BASE /afs/cern.ch/sw/lcg/external/geant4

 source $G4BASE/9.4.p02/x86_64-slc5-gcc43/setup.csh

 setenv HEPMC_DIR /afs/cern.ch/sw/lcg/external/HepMC/2.05.01/x86_64-slc5-gcc43-opt

 echo " " 
 echo "You have choosen the next HepMC version:" 
 echo "$HEPMC_DIR"

 setenv ROOTSYS /afs/cern.ch/sw/lcg/app/releases/ROOT/5.30.00/x86_64-slc5-gcc43-opt/root

 echo "  "
 echo "You have choosen the next ROOTSYS version:"
 echo "$ROOTSYS"

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

 setenv MyGeant ~/geant4/9.4.p02/x86_64-slc5-gcc43/bin/Linux-g++

 echo " "
 echo "Your geant4 directory (alias MyGeant) with executable fcalor:"
 echo "$MyGeant"
 echo " "


