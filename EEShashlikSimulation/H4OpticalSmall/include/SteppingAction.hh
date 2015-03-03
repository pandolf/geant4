// Martin Goettlich @ DESY
//

#ifndef SteppingAction_H
#define SteppingAction_H 1

#include "G4UserSteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"

#include "G4ProcessManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4UnitsTable.hh"

#include "EEShashDetectorConstruction.hh"
#include "TrackInformation.hh"

#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TRandom3.h"

class SteppingAction : public G4UserSteppingAction
{
public:
  
  SteppingAction(EEShashDetectorConstruction* detectorConstruction, const G4int& scint, const G4int& cher);
  ~SteppingAction();
  virtual void UserSteppingAction(const G4Step*);


private:
  EEShashDetectorConstruction* fDetectorConstruction;

  G4int propagateScintillation;
  G4int propagateCerenkov;

};

#endif
