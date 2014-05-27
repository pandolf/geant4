//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: EEShashDetectorConstruction.cc 77601 2013-11-26 17:08:44Z gcosmo $
// 
/// \file EEShashDetectorConstruction.cc
/// \brief Implementation of the EEShashDetectorConstruction class

#include "EEShashDetectorConstruction.hh"
#include "EEShashCalorimeterSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4TwoVector.hh"
#include "G4ExtrudedSolid.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal 
G4GlobalMagFieldMessenger* EEShashDetectorConstruction::fMagFieldMessenger = 0; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashDetectorConstruction::EEShashDetectorConstruction()
 : G4VUserDetectorConstruction(),
   fCheckOverlaps(true),
   fNofLayers(-1)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashDetectorConstruction::~EEShashDetectorConstruction()
{ 
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* EEShashDetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();
  
  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashDetectorConstruction::DefineMaterials()
{ 

  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;  
  G4double density; 
  G4int ncomponents, natoms;
  G4double fractionmass;

  G4String symbol;

  G4Element* F  = new G4Element("Fluor"   ,symbol="F" , z=9.,  a=18.998*g/mole);
  G4Element* Ce = new G4Element("Cerium"  ,symbol="Ce", z=58., a=140.11*g/mole);

  G4Element* B  = new G4Element("Boron"      ,symbol="B" , z=5. , a=10.81*g/mole);
  G4Element* Ge = new G4Element("Germanium"  ,symbol="Ge", z=32., a=72.64*g/mole);
  G4Element* O  = new G4Element("Oxygen"     ,symbol="O" , z=8. , a=16.00*g/mole);

  G4Element* N  = new G4Element("Nitrogen"     ,symbol="N" , z=7., a= 14.01*g/mole);

  G4Element* C  = new G4Element("Carbon"     ,symbol="C" , z=6. , a=12.0107*g/mole);
  G4Element* H  = new G4Element("Hydrogen"   ,symbol="H" , z=1. , a=1.00794*g/mole);


  
  G4Material* pAir = new G4Material("Air" , density= 1.290*mg/cm3, ncomponents=2);
  pAir->AddElement(N, fractionmass=0.7);
  pAir->AddElement(O, fractionmass=0.3);


  // absorber
  new G4Material("Tungsten", z=74., a=183.84*g/mole, density=19.25*g/cm3);
  //new G4Material("Lead", z=82., a=207.2*g/mole, density=11.34*g/cm3);

  // active material
  G4Material* sens = new G4Material("CeF3" , density= 6.16*g/cm3, ncomponents=2);
  sens->AddElement(Ce , natoms=1);
  sens->AddElement(F ,  natoms=3);
  
  // BGO
  G4Material* bgo = new G4Material("BGO" , density= 7.13*g/cm3, ncomponents=3);
  bgo->AddElement(B   , natoms=4);
  bgo->AddElement(Ge  , natoms=3);
  bgo->AddElement(O   , natoms=12);
  

  // fibres:
  G4Material* polystyrene = new G4Material("Polystyrene", density=1.05*g/cm3, ncomponents=2);
  polystyrene->AddElement(C , natoms=8);
  polystyrene->AddElement(H , natoms=8);
  
  G4Material* PMMA = new G4Material("PMMA", density=1.18*g/cm3, ncomponents=3);
  PMMA->AddElement(C , natoms=5);
  PMMA->AddElement(O , natoms=2);
  PMMA->AddElement(H , natoms=8);
  

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* EEShashDetectorConstruction::DefineVolumes()
{
  // Geometry parameters

  // Shashlik:
  fNofLayers = 10;
  G4double absThickness = 3.1*mm;
  G4double actThickness = 10.*mm;
  G4double calorSizeXY  = 24.*mm;

  G4double layerThickness = absThickness + actThickness;
  G4double calorThickness = fNofLayers * layerThickness;

  // BGO:
  G4double bgoLength = 240.*mm;
  //G4double bgoSizeXY = 25.*mm;


  // fibres:
  G4double fibreLength = 420.*mm;


  // world:
  G4double worldSizeXY = 1.2 * (3.*calorSizeXY);
  G4double worldSizeZ  = 2. * (fibreLength); 
  
  // Get materials
  G4Material* defaultMaterial = G4Material::GetMaterial("Air");
  //G4Material* defaultMaterial = G4Material::GetMaterial("Galactic");
  //G4Material* absMaterial     = G4Material::GetMaterial("Lead");
  G4Material* absMaterial     = G4Material::GetMaterial("Tungsten");
  G4Material* actMaterial     = G4Material::GetMaterial("CeF3");
  G4Material* bgoMaterial     = G4Material::GetMaterial("BGO");
  G4Material* fibreCoreMaterial  = G4Material::GetMaterial("Polystyrene");
  G4Material* fibreCladMaterial  = G4Material::GetMaterial("PMMA");

  
  if ( ! defaultMaterial || ! absMaterial || ! actMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("EEShashDetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }  
   
  //     
  // World
  //
  G4VSolid* worldS 
    = new G4Box("World",           // its name
                 worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size
                         
  G4LogicalVolume* worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material
                 "World");         // its name
                                   
  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 worldLV,          // its logical volume                         
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 
  
  //                               
  // Calorimeter
  //  

  std::vector<G4TwoVector> octagonFace;
  G4double chamferLength = 2.1*mm;
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2.                ,+calorSizeXY/2. - chamferLength) );// start from top left corner
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2. + chamferLength,+calorSizeXY/2.                ) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2. - chamferLength,+calorSizeXY/2.                ) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2.                ,+calorSizeXY/2. - chamferLength) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2.                ,-calorSizeXY/2. + chamferLength) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2. - chamferLength,-calorSizeXY/2.                ) );
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2. + chamferLength,-calorSizeXY/2.                ) );
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2.                ,-calorSizeXY/2. + chamferLength) );


  G4VSolid* calorimeterS
    = new G4ExtrudedSolid( "Calorimeter",            // its name
        octagonFace,
        fNofLayers*layerThickness/2.,
        G4TwoVector(0.,0.), 1.,
        G4TwoVector(0.,0.), 1.);
  //= new G4Box("Calorimeter",     // its name
  //             calorSizeXY/2, calorSizeXY/2, calorThickness/2); // its size
                         
  G4LogicalVolume* calorLV
    = new G4LogicalVolume(
                 calorimeterS,     // its solid
                 defaultMaterial,  // its material
                 "Calorimeter");   // its name
                                   
  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 calorLV,          // its logical volume                         
                 "Calorimeter",    // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 
   
  //                                 
  // Layer
  //

  G4VSolid* layerS 
    = new G4ExtrudedSolid( "Layer",            // its name
        octagonFace,
        layerThickness/2.,
        G4TwoVector(0.,0.), 1.,
        G4TwoVector(0.,0.), 1.);

  //= new G4Box("Layer",           // its name
  //             calorSizeXY/2, calorSizeXY/2, layerThickness/2); //its size

                         
  G4LogicalVolume* layerLV
    = new G4LogicalVolume(
                 layerS,           // its solid
                 defaultMaterial,  // its material
                 "Layer");         // its name

  new G4PVReplica(
                 "Layer",          // its name
                 layerLV,          // its logical volume
                 calorLV,          // its mother
                 kZAxis,           // axis of replication
                 fNofLayers,        // number of replica
                 layerThickness);  // witdth of replica
  
  //                               
  // Absorber
  //
  G4VSolid* absS 
    = new G4ExtrudedSolid( "Abs",            // its name
        octagonFace,
        absThickness/2.,
        G4TwoVector(0.,0.), 1.,
        G4TwoVector(0.,0.), 1.);
  //= new G4Box("Abs",            // its name
  //             calorSizeXY/2, calorSizeXY/2, absThickness/2); // its size
                         
  G4LogicalVolume* absLV
    = new G4LogicalVolume(
                 absS,        // its solid
                 absMaterial, // its material
                 "AbsLV");        // its name
                                   
   new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., -actThickness/2), // its position
                 absLV,       // its logical volume                         
                 "Abs",           // its name
                 layerLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 

  //                               
  // Active Material
  //
  G4VSolid* actS 
    = new G4ExtrudedSolid( "Act",            // its name
        octagonFace,
        actThickness/2.,
        G4TwoVector(0.,0.), 1.,
        G4TwoVector(0.,0.), 1.);
  //= new G4Box("Act",             // its name
  //             calorSizeXY/2, calorSizeXY/2, actThickness/2); // its size
                         
  G4LogicalVolume* actLV
    = new G4LogicalVolume(
                 actS,             // its solid
                 actMaterial,      // its material
                 "ActLV");         // its name
                                   
  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., absThickness/2), // its position
                 actLV,            // its logical volume                         
                 "Act",            // its name
                 layerLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 



  // fibre:

  G4double fibreRadius = 0.5*mm;
  G4double cladThickness = 0.02*mm;

  G4VSolid* fibreS
    = new G4Tubs("Fibre", 0, fibreRadius, fibreLength/2., 0, twopi);  

  G4LogicalVolume* fibreLV
    = new G4LogicalVolume(
                 fibreS,     // its solid
                 defaultMaterial,  // its material
                 "Fibre");   // its name
                                   
  //new G4PVPlacement(
  //               0,                // no rotation
  //               G4ThreeVector(),  // at (0,0,0)
  //               fibreLV,          // its logical volume                         
  //               "Fibre",    // its name
  //               worldLV,          // its mother  volume
  //               false,            // no boolean operation
  //               0,                // copy number
  //               fCheckOverlaps);  // checking overlaps 



  // fibre core:

  G4VSolid* fibreCoreS
    = new G4Tubs("FibreCore", 0, fibreRadius-cladThickness, fibreLength/2., 0, twopi);  

  G4LogicalVolume* fibreCoreLV
    = new G4LogicalVolume(
                 fibreCoreS,             // its solid
                 fibreCoreMaterial,      // its material
                 "FibreCore");         // its name


  // fibre clad:

  G4VSolid* fibreCladS
    = new G4Tubs("FibreClad", fibreRadius-cladThickness, fibreRadius, fibreLength/2., 0, twopi);  

  G4LogicalVolume* fibreCladLV
    = new G4LogicalVolume(
                 fibreCladS,             // its solid
                 fibreCladMaterial,      // its material
                 "FibreClad");         // its name


  // place core and clad inside the fibre volume
  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(), // its position
                 fibreCoreLV,            // its logical volume                         
                 "FibreCore",            // its name
                 fibreLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 
  
  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(), // its position
                 fibreCladLV,            // its logical volume                         
                 "FibreClad",            // its name
                 fibreLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 

  

  // and now place the 4 fibres:
  int fibreCopy=0;
  for( int ix=-1; ix<=1; ix+=2 ) {
    for( int iy=-1; iy<=1; iy+=2 ) {

      G4double xPos = ix*(calorSizeXY/2.-0.696);
      G4double yPos = iy*(calorSizeXY/2.-0.696);

      new G4PVPlacement(
                     0,                // no rotation
                     G4ThreeVector(xPos,yPos,(fibreLength-calorThickness)/2.), // its position
                     fibreLV,            // its logical volume                         
                     "Fibre",            // its name
                     worldLV,          // its mother  volume
                     false,            // no boolean operation
                     fibreCopy++,                // copy number
                     fCheckOverlaps);  // checking overlaps 

    }
  }
 
  //
  // print parameters
  //
  G4cout << "\n------------------------------------------------------------"
         << "\n---> The calorimeter is " << fNofLayers << " layers of: [ "
         << absThickness/mm << "mm of " << absMaterial->GetName() 
         << " + "
         << actThickness/mm << "mm of " << actMaterial->GetName() << " ] " 
         << "\n------------------------------------------------------------\n";


  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);
  fibreLV->SetVisAttributes (G4VisAttributes::Invisible);
  //calorLV->SetVisAttributes (G4VisAttributes::Invisible);


  G4VisAttributes* grayBox= new G4VisAttributes(G4Colour(105,105,105));
  grayBox->SetForceSolid(true);
  absLV->SetVisAttributes(grayBox);

  G4VisAttributes* cyanBox= new G4VisAttributes(G4Colour(0,255,255));
  cyanBox->SetForceSolid(true);
  actLV->SetVisAttributes(cyanBox);

  G4VisAttributes* magentaBox= new G4VisAttributes(G4Colour(255,0,0));
  magentaBox->SetForceSolid(true);
  fibreCoreLV->SetVisAttributes(magentaBox);
  fibreCladLV->SetVisAttributes(magentaBox);



  //  
  // BGO
  //  

  G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  //matrixLV->SetVisAttributes(simpleBoxVisAtt);
  calorLV->SetVisAttributes(simpleBoxVisAtt);

  // create the matrix:

  int copyNumber = 0;
  int totalCopies = 8;

  G4VSolid* bgoS 
     = new G4Box("BGOChannel",     // its name
                calorSizeXY/2, calorSizeXY/2, bgoLength/2); // its size
                         
  G4LogicalVolume* bgoLV 
     = new G4LogicalVolume(
                bgoS,     // its solid
                bgoMaterial,  // its material
                "BGOChannel");   // its name

  bgoLV->SetVisAttributes(simpleBoxVisAtt);
                                       

  for( int ix=-1; ix<=1; ++ix ) {

    for( int iy=-1; iy<=1; ++iy ) {


      if( ix==0 && iy==0 ) continue;  // central channel is the shash

      if( copyNumber>=totalCopies ) {
        std::cout << "--> ERROR!! Trying to instantiate a number of copies which exceeds totalCopies (=" << totalCopies << std::endl;
        std::cout << " Exiting." << std::endl;
        exit(11);
      }

      G4double zPos = (bgoLength-calorThickness)/2.;
      G4double miniGap = 1.*mm;
      G4double xPos = ix*(calorSizeXY + miniGap);
      G4double yPos = iy*(calorSizeXY + miniGap);

      new G4PVPlacement(
                     0,                // no rotation
                     G4ThreeVector(xPos, yPos, zPos),  // at (0,0,0)
                     bgoLV,          // its logical volume                         
                     "BGOChannel",    // its name
                     worldLV,          // its mother  volume
                     false,            // no boolean operation
                     copyNumber,                // copy number
                     fCheckOverlaps);  // checking overlaps 

      copyNumber += 1;

    }
  }
      

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashDetectorConstruction::ConstructSDandField()
{
  // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // 
  // Sensitive detectors
  //
  //
  // First the actual Shashlik:
  EEShashCalorimeterSD* absSD 
    = new EEShashCalorimeterSD("AbsSD", "AbsHitsCollection", fNofLayers);
  SetSensitiveDetector("AbsLV",absSD);

  EEShashCalorimeterSD* actSD 
    = new EEShashCalorimeterSD("ActSD", "ActHitsCollection", fNofLayers);
  SetSensitiveDetector("ActLV",actSD);


  //// then the surrounding BGO matrix:
  //SetSensitiveDetector("bgoLV",bgoSD);

  // 
  // Magnetic field
  //
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue = G4ThreeVector();
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);
  
  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
