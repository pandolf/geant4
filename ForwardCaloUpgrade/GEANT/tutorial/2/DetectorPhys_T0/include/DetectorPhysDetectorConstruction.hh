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

#ifndef DetectorPhysDetectorConstruction_h
#define DetectorPhysDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"


class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorPhysDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    
    DetectorPhysDetectorConstruction();
    ~DetectorPhysDetectorConstruction();
    
  G4VPhysicalVolume* Construct();
  
private:
  
  //***********  World

  G4LogicalVolume*   logicWorld;    
  G4VPhysicalVolume* physiWorld;   
  
  //***********  Box 
  G4LogicalVolume* LogicalChamberGasBox;
  G4VPhysicalVolume *  PhysicalChamberGasBox; 

  //***********  Material
  G4Material* Vaccum;
  
private:
    
    void DefineMaterials();
    G4VPhysicalVolume* ConstructSetUp();     
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
