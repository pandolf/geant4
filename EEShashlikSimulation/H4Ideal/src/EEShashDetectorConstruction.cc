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

EEShashDetectorConstruction::EEShashDetectorConstruction( G4double rotation, G4double zTras )
 : G4VUserDetectorConstruction(),
   fCheckOverlaps(true),
   fNofLayers(-1),
   fNofBGOs(-1),
   fRotation(rotation),
   fZtraslation(zTras)
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

  //For Pompom
  G4Material* POM = new G4Material("POM", density=1.415*g/cm3, ncomponents=3);
  POM->AddElement(C , natoms=1);
  POM->AddElement(O , natoms=1);
  POM->AddElement(H , natoms=2);
  

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
  fNofLayers = 15;
  G4double absThickness = 3.1*mm;
  G4double actThickness = 10.*mm;
  G4double calorSizeXY  = 24.*mm;

  G4double layerThickness = absThickness + actThickness;
  G4double calorThickness = fNofLayers * layerThickness;
  fZtraslation += calorThickness/2.;

  G4double rotationDist = 357.*mm; //Because matrix doesn't start at box edge

  // BGO:
  fNofBGOs = 24;
  G4double bgoLength = 240.*mm;
  //G4double bgoSizeXY = 25.*mm;


  // fibres:
  G4double fibreLength = 420.*mm;

  // Weird plastic piece at beginning of shashlik, I shall name it PomPom
  G4double pompomSizeXY = 24 *mm;
  G4double pompomLength = 8*mm;


  // Hodoscope 8x8x1mm:
  G4double hodoSizeXY = 8.5*mm;
  //G4double hodoSizeXY = 10.*mm;
  G4double hodoLength = 4.*mm;
  G4double hodoDistance = 30.*mm;


  //Starting from the Calobox
  //Scintillator 3 
  G4double scint3SizeXY = 50.*mm;
  G4double scint3Length = 10.*mm;
  G4double scint3Distance = 2950.*mm;


  // Hodoscope 11 
  G4double hodo21Distance = (3000+1400+140+640.+110.+190.+130.)*mm;
  //Hodoscope 12
  G4double hodo22Distance = (3000+1400+140+640.+110.+190.+130+350.)*mm;


  //The group of 3 scintillators
  //Scint 21
  G4double scint21SizeXY = 50.*mm;
  G4double scint21Length = 5.*mm;
  G4double scint21Distance = (3000+1400+140+640.)*mm;
  //Scint 22 Tiny little scint
  G4double scint22SizeXY = 10.*mm;
  G4double scint22Length = 1.*mm;
  G4double scint22Distance = (3000+1400+140+640.+110.)*mm;
  //Scint 23 
  G4double scint23SizeXY = 30.*mm;
  G4double scint23Length = 3.*mm;
  G4double scint23Distance = (3000+1400+140+640.+110.+190.)*mm;

  //Last (or first hodo from beam) hodoscope? It's a scintillator, not readout
  G4double hodo31SizeXY = 100.*mm;
  G4double hodo31Length = 5.*mm;
  G4double hodo31Distance = (3000+1400+140+640.+110.+190.+350.+530.+90.)*mm;  
  G4double hodo32Distance = (3000+1400+140+640.+110.+190.+350.+530.+90.+1.+5.)*mm;

 //Scintillator X (weird roundish on top, approxed as square) 
  G4double scintXSizeXY = 50.*mm;
  G4double scintXLength = 4.*mm;
  G4double scintXDistance = (3000+1400+140+640.+110.+190.+130+350.+530.+1400.+1220.)*mm;

  // Hodoscope 21 
  G4double hodo11SizeXY = 30.*mm;
  G4double hodo11Length = 2.*mm;
  G4double hodo11Distance = 3130.*mm;
  //Hodoscope 22
  G4double hodo12Distance = 3490.*mm;


  // world:
  G4double worldSizeXY = 20. * (5.*calorSizeXY);
  G4double worldSizeZ  = 100. * (fibreLength); 
  //G4double worldSizeXY = 1.2 * (3.*calorSizeXY);
  //G4double worldSizeZ = 2. * (fibreLength); 
  
  // Get materials
  G4Material* defaultMaterial = G4Material::GetMaterial("Air");
  //G4Material* defaultMaterial = G4Material::GetMaterial("Galactic");
  //G4Material* absMaterial     = G4Material::GetMaterial("Lead");
  G4Material* absMaterial     = G4Material::GetMaterial("Tungsten");
  G4Material* actMaterial     = G4Material::GetMaterial("CeF3");
  G4Material* bgoMaterial     = G4Material::GetMaterial("BGO");
  G4Material* fibreCoreMaterial  = G4Material::GetMaterial("Polystyrene");
  G4Material* fibreCladMaterial  = G4Material::GetMaterial("PMMA");

  G4Material* scintMaterial = G4Material::GetMaterial("PMMA");
  G4Material* hodoMaterial    = G4Material::GetMaterial("PMMA");
  G4Material* pompomMaterial = G4Material::GetMaterial("POM");


  
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
  


  G4VSolid* labS 
    = new G4Box("lab",           // its name
                 worldSizeXY/4, worldSizeXY/4, worldSizeZ/4); // its size
                         
  G4LogicalVolume* labLV
    = new G4LogicalVolume(
                 labS,           // its solid
                 defaultMaterial,  // its material
                 "lab");         // its name


  G4RotationMatrix* rotation = new G4RotationMatrix();
  rotation->rotateX(fRotation*3.14159265359/180.);

  G4RotationMatrix* rotationNeg = new G4RotationMatrix();
  rotationNeg->rotateX(-fRotation*3.14159265359/180.);
                                   
  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 labLV,          // its logical volume                         
                 "lab",          // its name
                 worldLV,                // its mother  volume
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
                 rotation,                // no rotation
                 G4ThreeVector(0., sin(fRotation*3.14159265359/180.)*calorThickness/2 ,  cos(-fRotation*3.14159265359/180.)* calorThickness/2.),  // its position
                 calorLV,          // its logical volume                         
                 "Calorimeter",    // its name
                 labLV,          // its mother  volume
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
                 layerThickness);  // width of replica
  
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
  //               labLV,          // its mother  volume
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
                 "FibreCoreLV");         // its name


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
                 G4ThreeVector(0., 0., 0.), // its position
                 fibreCoreLV,            // its logical volume                         
                 "FibreCoreLV",            // its name
                 fibreLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 
  
  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., 0.), // its position
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
      G4double yPos = iy*(calorSizeXY/2.-0.696) + sin(fRotation*3.14159265359/180.)*sqrt(((fibreLength-calorThickness)/2.+calorThickness/2.)*((fibreLength-calorThickness)/2.+calorThickness/2) + xPos*xPos) ;

      new G4PVPlacement(
                     rotation,                // no rotation
                     G4ThreeVector(xPos,yPos,  cos(-fRotation*3.14159265359/180.)*((fibreLength-calorThickness)/2.+calorThickness/2.)  - sin(fRotation*3.14159265359/180.)*(iy*(calorSizeXY/2.-0.696))    ), // its position
                     fibreLV,            // its logical volume                         
                     "Fibre",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     fibreCopy++,                // copy number
                     fCheckOverlaps);  // checking overlaps 

    }
  }
 


  //
  // PomPom or Pommie 
  //
  G4VSolid* PompomS
    = new G4Box( "Pompom",            // its name
		 pompomSizeXY/2., pompomSizeXY/2., pompomLength/2.); //its size

                   
  G4LogicalVolume* PompomLV
     = new G4LogicalVolume(
                PompomS,     // its solid
                pompomMaterial,  // its material
                "PompomLV");   // its name

      new G4PVPlacement(
                     rotation,                // no rotation
                     G4ThreeVector(0. , -sin(fRotation*3.14159265359/180.)*( pompomLength/2.)  ,  cos(-fRotation*3.14159265359/180.)*( -pompomLength/2.) ), // its position
                     PompomLV,            // its logical volume                         
                     "Pompom",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 

  //
  // Hodoscope 
  //
  G4VSolid* HodoS
    = new G4Box( "Hodoscope",            // its name
		 hodoSizeXY/2., hodoSizeXY/2., hodoLength/2.); //its size

                   
  G4LogicalVolume* HodoLV 
     = new G4LogicalVolume(
                HodoS,     // its solid
                hodoMaterial,  // its material
                "HodoLV");   // its name
  
      new G4PVPlacement(
                     rotation,                // no rotation
                     G4ThreeVector(0.,sin(fRotation*3.14159265359/180.)*(-hodoLength/2.-hodoDistance),  cos(-fRotation*3.14159265359/180.)*(-hodoLength/2.-hodoDistance) ), // its position
                     HodoLV,            // its logical volume                         
                     "Hodoscope",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 


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
  //Scintillators 3
  //
  G4VSolid* Scint3S
    = new G4Box( "Scintillator3",            // its name
		 scint3SizeXY/2., scint3SizeXY/2., scint3Length/2.); //its size
              
  G4LogicalVolume* Scint3LV 
     = new G4LogicalVolume(
                Scint3S,     // its solid
                scintMaterial,  // its material
                "Scint3LV");   // its name

      new G4PVPlacement(
                     0,                // no rotation
                     G4ThreeVector(0.,0.,  -scint3Distance -scint3Length/2. -rotationDist), // its position
                     Scint3LV,            // its logical volume                         
                     "Scintillator3",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 
 




  //
  //Scintillator X
  //
  G4VSolid* ScintXS
    = new G4Box( "ScintillatorX",            // its name
		 scintXSizeXY/2., scintXSizeXY/2., scintXLength/2.); //its size
              
  G4LogicalVolume* ScintXLV 
     = new G4LogicalVolume(
                ScintXS,     // its solid
                scintMaterial,  // its material
                "ScintXLV");   // its name

      new G4PVPlacement(
                     0,                // no rotation
                     G4ThreeVector(0., 0., -scintXDistance-scintXLength/2. -rotationDist), // its position
                     ScintXLV,            // its logical volume                         
                     "ScintillatorX",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 
  


  //
  // Hodoscope 11
  //
  G4VSolid* Hodo11S
    = new G4Box( "Hodoscope11",            // its name
		 hodo11SizeXY/2., hodo11SizeXY/2., hodo11Length/2.); //its size

                   
  G4LogicalVolume* Hodo11LV 
     = new G4LogicalVolume(
                Hodo11S,     // its solid
                hodoMaterial,  // its material
                "Hodo11LV");   // its name

      new G4PVPlacement(
                     0,                // no rotation
                     G4ThreeVector(0., 0,fZtraslation-calorThickness/2.-hodo11Distance-hodo11Length/2. -rotationDist), // its position
                     Hodo11LV,            // its logical volume                         
                     "Hodoscope11",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 


  //
  // Hodoscope 12
  //
      new G4PVPlacement(
                     0,                // no rotation
                     G4ThreeVector(0.,0.,fZtraslation-calorThickness/2.-hodo12Distance-hodo11Length/2. -rotationDist), // its position
                     Hodo11LV,            // its logical volume                         
                     "Hodoscope12",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 


  //
  //Scintillators 2.1
  //
  G4VSolid* Scint21S
    = new G4Box( "Scintillator21",            // its name
		 scint21SizeXY/2., scint21SizeXY/2., scint21Length/2.); //its size
              
  G4LogicalVolume* Scint21LV 
     = new G4LogicalVolume(
                Scint21S,     // its solid
                scintMaterial,  // its material
                "Scint21LV");   // its name

      new G4PVPlacement(
                     0,                // no rotation
                     G4ThreeVector(0., 0.,fZtraslation-calorThickness/2.-scint21Distance-scint21Length/2. -rotationDist), // its position
                     Scint21LV,            // its logical volume                         
                     "Scintillator21",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 
 

  //
  //Scintillators 2.2
  //
  G4VSolid* Scint22S
    = new G4Box( "Scintillator22",            // its name
		 scint22SizeXY/2., scint22SizeXY/2., scint22Length/2.); //its size
              
  G4LogicalVolume* Scint22LV 
     = new G4LogicalVolume(
                Scint22S,     // its solid
                scintMaterial,  // its material
                "Scint22LV");   // its name

      new G4PVPlacement(
                     0,                // no rotation
                     G4ThreeVector(0.,0. ,fZtraslation-calorThickness/2.-scint22Distance-scint22Length/2.-rotationDist), // its position
                     Scint22LV,            // its logical volume                         
                     "Scintillator22",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 
   
  //
  //Scintillators 2.3
  //
  G4VSolid* Scint23S
    = new G4Box( "Scintillator23",            // its name
		 scint23SizeXY/2., scint23SizeXY/2., scint23Length/2.); //its size
              
  G4LogicalVolume* Scint23LV 
     = new G4LogicalVolume(
                Scint23S,     // its solid
                scintMaterial,  // its material
                "Scint23LV");   // its name

      new G4PVPlacement(
                     0,                // no rotation
                     G4ThreeVector(0., 0. ,fZtraslation-calorThickness/2.-scint23Distance-scint23Length/2.-rotationDist), // its position
                     Scint23LV,            // its logical volume                         
                     "Scintillator23",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 
   

  //
  // Hodoscope 2.1
  //

      new G4PVPlacement(
                     0,                // no rotation
                     G4ThreeVector(0., 0.,fZtraslation-calorThickness/2.-hodo21Distance-hodo11Length/2.-rotationDist), // its position
                     Hodo11LV,            // its logical volume                         
                     "Hodoscope11",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 
  //
  // Hodoscope 22
  //
      new G4PVPlacement(
		     0,                // no rotation
                     G4ThreeVector(0., 0.,fZtraslation-calorThickness/2.-hodo22Distance-hodo11Length/2.+rotationDist), // its position
                     Hodo11LV,            // its logical volume                         
                     "Hodoscope12",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 





  //
  // Hodoscope 3.1
  //
  G4VSolid* Hodo31S
    = new G4Box( "Hodoscope31",            // its name
		 hodo31SizeXY/2., hodo31SizeXY/2., hodo31Length/2.); //its size

                   
  G4LogicalVolume* Hodo31LV 
     = new G4LogicalVolume(
                Hodo31S,     // its solid
                hodoMaterial,  // its material
                "Hodo31LV");   // its name

      new G4PVPlacement(
                     0,                // no rotation
                     G4ThreeVector(0.,0.,fZtraslation-calorThickness/2.-hodo31Distance-hodo31Length/2.-rotationDist), // its position
                     Hodo31LV,            // its logical volume                 
                     "Hodoscope31",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 
  //
  // Hodoscope 3.2
  //
      new G4PVPlacement(
                     0,                // no rotation
                     G4ThreeVector(0.,0.,fZtraslation-calorThickness/2.-hodo32Distance-hodo31Length/2.-rotationDist), // its position
                     Hodo31LV,            // its logical volume                         
                     "Hodoscope32",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
                     fCheckOverlaps);  // checking overlaps 






  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);
  fibreLV->SetVisAttributes (G4VisAttributes::Invisible);
  labLV->SetVisAttributes (G4VisAttributes::Invisible);


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

  G4VisAttributes* pomBox= new G4VisAttributes(G4Colour(0,0,255));
  pomBox->SetForceSolid(true);
  PompomLV->SetVisAttributes(pomBox);

  G4VisAttributes* violetBox= new G4VisAttributes(G4Colour(200,0,255));
  violetBox->SetForceSolid(true);
  ScintXLV->SetVisAttributes(violetBox);
  Scint21LV->SetVisAttributes(violetBox);
  Scint22LV->SetVisAttributes(violetBox);
  Scint23LV->SetVisAttributes(violetBox);
  Scint3LV->SetVisAttributes(violetBox);
 
  G4VisAttributes* violet2Box= new G4VisAttributes(G4Colour(150,0,255));
  violet2Box->SetForceSolid(true);
  Hodo11LV->SetVisAttributes(violet2Box);
  HodoLV->SetVisAttributes(violet2Box);
  Hodo31LV->SetVisAttributes(violet2Box);




  //  
  // BGO
  //  

  G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(0,0,0));
  simpleBoxVisAtt->SetVisibility(true);
  //matrixLV->SetVisAttributes(simpleBoxVisAtt);
  calorLV->SetVisAttributes(simpleBoxVisAtt);

  // create the matrix:

  //  int copyNumber = 0;
  //  int totalCopies = fNofBGOs;

  G4VSolid* bgoS 
     = new G4Box("BGOChannel",     // its name
                calorSizeXY/2, calorSizeXY/2, bgoLength/2); // its size
                         
  G4LogicalVolume* bgoLV 
     = new G4LogicalVolume(
                bgoS,     // its solid
                bgoMaterial,  // its material
                "BGOLV");   // its name
  /*
  bgoLV->SetVisAttributes(simpleBoxVisAtt);
                                       
  for( int ix=-2; ix<=2; ++ix ) {
    for( int iy=-2; iy<=2; ++iy ) {
      if( ix==0 && iy==0 ) continue;  // central channel is the shash
      if( copyNumber>=totalCopies ) {
        std::cout << "--> ERROR!! Trying to instantiate a number of copies which exceeds totalCopies (=" << totalCopies << std::endl;
        std::cout << " Exiting." << std::endl;
        exit(11);
      }
      G4double zPos = (bgoLength-calorThickness)/2.;
      G4double miniGap = 1.5*mm;
      G4double xPos = ix*(calorSizeXY + miniGap) ;
      G4double yPos = iy*(calorSizeXY + miniGap) + sin(fRotation*3.14159265359/180.)*( zPos + fZtraslation) ;
      new G4PVPlacement(
                     rotation,                // no rotation
                     G4ThreeVector(xPos, yPos, cos(-fRotation*3.14159265359/180.)*(zPos + fZtraslation)  - sin(fRotation*3.14159265359/180.)*( iy*(calorSizeXY + miniGap)) ),  // at (0,0,0)
                     bgoLV,          // its logical volume                         
                     "BGOChannel",    // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     copyNumber,                // copy number
                     fCheckOverlaps);  // checking overlaps 
      copyNumber += 1;
    }
  }
      
  */
  int copyNumber = 1;
  int totalCopies = 25;

  for( int ix=-2; ix<=2; ++ix ) {
    for( int iy=-2; iy<=2; ++iy ) {
      if( ix==0 && iy==0 ) continue;  // central channel is the shash

      if( copyNumber>=totalCopies ) {
        std::cout << "--> ERROR!! Trying to instantiate a number of copies which exceeds totalCopies (=" << totalCopies << std::endl;
        std::cout << " Exiting." << std::endl;
        exit(11);  }

      G4double miniGap = 1.5*mm;


      //  G4double yPos = iy*(calorSizeXY/2.-0.696) + sin(fRotation*3.14159265359/180.)*sqrt(((fibreLength-calorThickness)/2.+calorThickness/2.)*((fibreLength-calorThickness)/2.+calorThickness/2) + xPos*xPos) ;

      //    G4ThreeVector(xPos,yPos,  cos(-fRotation*3.14159265359/180.)*((fibreLength-calorThickness)/2.+calorThickness/2.)  - sin(fRotation*3.14159265359/180.)*(iy*(calorSizeXY/2.-0.696))    ), // its position


      G4double zPos = (calorThickness-calorThickness)/2.;
      G4double xPos = ix*(calorSizeXY + miniGap) ;
      G4double yPos = iy*(calorSizeXY + miniGap) + sin(fRotation*3.14159265359/180.)*( zPos + fZtraslation) ;
      G4double yPosPom = iy*(calorSizeXY + miniGap) + sin(fRotation*3.14159265359/180.)*sqrt( (zPos + fZtraslation -calorThickness/2.- pompomLength/2.)*(zPos + fZtraslation -calorThickness/2.- pompomLength/2.) + xPos*xPos) ;

      new G4PVPlacement(
                     rotation,                // rotation
                     G4ThreeVector(xPos, yPos, cos(-fRotation*3.14159265359/180.)*(zPos + fZtraslation)  - sin(fRotation*3.14159265359/180.)*( iy*(calorSizeXY + miniGap)) ),
                     calorLV,          // its logical volume                         
                     "Calorimeter",    // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     copyNumber,                // copy number
                     fCheckOverlaps);  // checking overlaps 

     new G4PVPlacement(
                     rotation,                // no rotation
                     G4ThreeVector(xPos, yPosPom, cos(-fRotation*3.14159265359/180.)*(zPos + fZtraslation -calorThickness/2.- pompomLength/2.)  - sin(fRotation*3.14159265359/180.)*( iy*(calorSizeXY + miniGap)) ),
                     PompomLV,            // its logical volume                         
                     "Pompom",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     0,                // copy number
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
    = new EEShashCalorimeterSD("AbsSD", "AbsHitsCollection", fNofLayers,1);
  SetSensitiveDetector("AbsLV",absSD);

  EEShashCalorimeterSD* actSD 
    = new EEShashCalorimeterSD("ActSD", "ActHitsCollection", fNofLayers,1);
  SetSensitiveDetector("ActLV",actSD);

  // then the surrounding BGO matrix:
  EEShashCalorimeterSD* bgoSD 
    = new EEShashCalorimeterSD("BgoSD", "BgoHitsCollection", fNofBGOs,1);
  SetSensitiveDetector("BGOLV",bgoSD);

  // then the fibres
  EEShashCalorimeterSD* fibrSD 
    = new EEShashCalorimeterSD("FibrSD", "FibrHitsCollection", 4,1);
  SetSensitiveDetector("FibreCoreLV",fibrSD);

  // and  POMMMELS:
  EEShashCalorimeterSD* pompomSD 
    = new EEShashCalorimeterSD("PompomSD", "PompomHitsCollection", 1,1);
  SetSensitiveDetector("PompomLV",pompomSD);

  // and now also the HODOR
    EEShashCalorimeterSD* hodoSD 
    = new EEShashCalorimeterSD("HodoSD", "HodoHitsCollection", 1,1);
  SetSensitiveDetector("HodoLV",hodoSD);

 // then there is also the Scintillator 3
  EEShashCalorimeterSD* scint3SD 
    = new EEShashCalorimeterSD("Scint3SD", "Scint1HitsCollection", 1,1);
  SetSensitiveDetector("Scint3LV",scint3SD);

  // and now also the HODOR 11
  EEShashCalorimeterSD* hodo11SD 
    = new EEShashCalorimeterSD("Hodo11SD", "Hodo11HitsCollection", 1,1);
  SetSensitiveDetector("Hodo11LV",hodo11SD);

  // and now also the HODOR 12
  EEShashCalorimeterSD* hodo12SD 
    = new EEShashCalorimeterSD("Hodo12SD", "Hodo12HitsCollection", 1,1);
  SetSensitiveDetector("Hodo11LV",hodo12SD);




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
