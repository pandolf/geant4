// ********************************************************************
// *                                                                  *
// *         Experimentelle Methoden der Teilchenphysik               *
// *                                                                  *
// * Geant 4 Tutorials  . Detector physics                            *
// * RWTH Aachen 22.10.2009                                           *
// * Karim LAIHEM                                                     *
// ********************************************************************
//

#ifndef DetectorPhysVisManager_h
#define DetectorPhysVisManager_h 1

#ifdef G4VIS_USE

#include "G4VisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorPhysVisManager: public G4VisManager {

public:

  DetectorPhysVisManager ();

private:

  void RegisterGraphicsSystems ();

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

#endif
