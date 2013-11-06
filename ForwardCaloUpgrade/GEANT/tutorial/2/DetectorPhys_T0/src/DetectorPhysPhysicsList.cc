// ********************************************************************
// *                                                                  *
// *         Experimentelle Methoden der Teilchenphysik               *
// *                                                                  *
// * Geant 4 Tutorials  . Detector physics                            *
// * RWTH Aachen 22.10.2009                                           *
// * Karim LAIHEM                                                     *
// ********************************************************************
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorPhysPhysicsList.hh"
#include "G4ParticleTypes.hh"
          

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorPhysPhysicsList::DetectorPhysPhysicsList():  G4VUserPhysicsList()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorPhysPhysicsList::~DetectorPhysPhysicsList()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorPhysPhysicsList::ConstructParticle()
{
  G4Electron::ElectronDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void DetectorPhysPhysicsList::ConstructProcess()
{
  AddTransportation();
}




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorPhysPhysicsList::SetCuts()
{

  G4int temp = GetVerboseLevel();                                                
  SetVerboseLevel(0);                                                           
  SetCutsWithDefault();   
  SetVerboseLevel(temp);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

