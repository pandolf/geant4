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

#ifndef DetectorPhysPhysicsList_h
#define DetectorPhysPhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorPhysPhysicsList: public G4VUserPhysicsList

{
  public:
    DetectorPhysPhysicsList();
   ~DetectorPhysPhysicsList();

  protected:
    // Construct particle and physics
    void ConstructParticle();
    void ConstructProcess();
    void SetCuts();


private:
  G4int VerboseLevel;
//  G4int OpVerbLevel;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif



