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

EEShashDetectorConstruction::EEShashDetectorConstruction( G4int nLayers, std::string actType, G4double actThickness, G4double absThickness, G4double rotation, G4double zTras )
 : G4VUserDetectorConstruction(),
   fCheckOverlaps(true),
   fNofLayers(nLayers),
   fRotation(rotation),
   fZtraslation(zTras),
   fActThickness(actThickness*mm),
   fAbsThickness(absThickness*mm),
   fActType(actType),
 fNLtot(40),fNRtot(50),fDLradl(0.5),fDRradl(0.1),
 fDLlength(0.),fDRlength(0.),
 fEcalLength(0.),fEcalRadius(0.)
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

  G4Element* B  = new G4Element("Boron"      ,symbol="B" , z=5. , a=10.81*g/mole);
  G4Element* Ge = new G4Element("Germanium"  ,symbol="Ge", z=32., a=72.64*g/mole);
  G4Element* O  = new G4Element("Oxygen"     ,symbol="O" , z=8. , a=16.00*g/mole);

  G4Element* N  = new G4Element("Nitrogen"     ,symbol="N" , z=7., a= 14.01*g/mole);

  G4Element* C  = new G4Element("Carbon"     ,symbol="C" , z=6. , a=12.0107*g/mole);
  G4Element* H  = new G4Element("Hydrogen"   ,symbol="H" , z=1. , a=1.00794*g/mole);

  G4Element* Al = new G4Element("Aliminum",symbol="Al", z=13., a= 26.98*g/mole);
  G4Element* Si = new G4Element("Silicon" ,symbol="Si", z=14., a= 28.08*g/mole);
  G4Element* Cl = new G4Element("Chlorine",symbol="Cl", z=17., a= 35.45*g/mole);
  G4Element* Cu = new G4Element("Copper"  ,symbol="Cu", z=29., a= 63.54*g/mole);
  G4Element* Zn = new G4Element("Zinc"    ,symbol="Zn", z=30., a= 65.38*g/mole);
  G4Element* Y  = new G4Element("Ytrium"  ,symbol="Y" , z=39., a= 88.91*g/mole);
  G4Element* Ce = new G4Element("Cerium"  ,symbol="Ce", z=58., a=140.11*g/mole);
  G4Element* Lu = new G4Element("Lutecium",symbol="Lu", z=71., a=174.96*g/mole);
  G4Element* W  = new G4Element("Tungsten",symbol="W" , z=74., a=183.84*g/mole);
  G4Element* Pb = new G4Element("Lead"    ,symbol="Pb", z=82., a=207.20*g/mole);
  
  
  G4Material* Air = new G4Material("Air" , density= 1.290*mg/cm3, ncomponents=2);
  Air->AddElement(N, fractionmass=0.7);
  Air->AddElement(O, fractionmass=0.3);


  // absorber
  new G4Material("Tungsten", z=74., a=183.84*g/mole, density=19.25*g/cm3);
  //new G4Material("Lead", z=82., a=207.2*g/mole, density=11.34*g/cm3);

  // active material
  G4Material* CeF3 = new G4Material("CeF3" , density= 6.16*g/cm3, ncomponents=2);
  CeF3->AddElement(Ce , natoms=1);
  CeF3->AddElement(F ,  natoms=3);
  

  G4Material* LYSO = new G4Material("LYSO" , density= 7.30*g/cm3, ncomponents=4);
  LYSO->AddElement(Lu, 71.45*perCent );
  LYSO->AddElement(Y ,  4.03*perCent );
  LYSO->AddElement(Si,  6.37*perCent );
  LYSO->AddElement(O , 18.15*perCent );



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
  //fNofLayers = 28;
  //G4double absThickness = 2.5*mm;
  ////  G4double absThickness = 3.1*mm;
  //G4double actThickness = 1.5*mm;
  ////  G4double actThickness = 10.*mm;
  G4double calorSizeXY  = 2000.*mm;

  G4double layerThickness = fAbsThickness + fActThickness;
  G4double calorThickness = fNofLayers * layerThickness;
  fZtraslation += calorThickness/2.;




  // world:
  G4double worldSizeXY = 10. * (10.*calorSizeXY);
  G4double worldSizeZ  = 10. * (90000.); 
  //G4double worldSizeXY = 1.2 * (3.*calorSizeXY);
  //G4double worldSizeZ = 2. * (fibreLength); 
  
  // Get materials
  // G4Material* defaultMaterial = G4Material::GetMaterial("Air");
  G4Material* defaultMaterial = G4Material::GetMaterial("Galactic");
  //G4Material* absMaterial     = G4Material::GetMaterial("Lead");
  G4Material* absMaterial     = G4Material::GetMaterial("Tungsten");
  G4Material* actMaterial     = G4Material::GetMaterial(fActType);
  //G4Material* actMaterial     = G4Material::GetMaterial("LYSO");
  //  G4Material* actMaterial     = G4Material::GetMaterial("CeF3");


  
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

                               
  new G4PVPlacement(
                 rotation,                // no rotation
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

  G4VSolid* calorimeterS 
    = new G4Box( "Calorimeter",            // its name
		 calorSizeXY/2., calorSizeXY/2., fNofLayers*layerThickness/2.);

                    
  G4LogicalVolume* calorLV
    = new G4LogicalVolume(
                 calorimeterS,     // its solid
                 defaultMaterial,  // its material
                 "Calorimeter");   // its name
                                   
  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., fZtraslation),  // its position
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
    = new G4Box( "Layer",            // its name
  calorSizeXY/2., calorSizeXY/2.,     layerThickness/2. );

                  
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
                 fNofLayers,       // number of replica
                 layerThickness);  // witdth of replica
 
  //                               
  // Absorber
  //
  G4VSolid* absS 
    = new G4Box( "Abs",            // its name
		 calorSizeXY/2., calorSizeXY/2.,       fAbsThickness/2. );
                  
  G4LogicalVolume* absLV
    = new G4LogicalVolume(
                 absS,        // its solid
                 absMaterial, // its material
                 "AbsLV");        // its name
                                   
   new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., -fActThickness/2), // its position
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
    = new G4Box( "Act",            // its name
		 calorSizeXY/2., calorSizeXY/2.,  fActThickness/2.);

                      
  G4LogicalVolume* actLV
    = new G4LogicalVolume(
                 actS,             // its solid
                 actMaterial,      // its material
                 "ActLV");         // its name
                                   
  ecalVolume = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., fAbsThickness/2), // its position
                 actLV,            // its logical volume                         
                 "Act",            // its name
                 layerLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 





  

  
 
  //
  // print parameters
  //
  G4cout << "\n------------------------------------------------------------"
         << "\n---> The calorimeter is " << fNofLayers << " layers of: [ "
         << fAbsThickness/mm << "mm of " << absMaterial->GetName() 
         << " + "
         << fActThickness/mm << "mm of " << actMaterial->GetName() << " ] " 
         << "\n------------------------------------------------------------\n";
             

 


  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);
  labLV->SetVisAttributes (G4VisAttributes::Invisible);


  G4VisAttributes* grayBox= new G4VisAttributes(G4Colour(105,105,105));
  grayBox->SetForceSolid(true);
  absLV->SetVisAttributes(grayBox);

  G4VisAttributes* cyanBox= new G4VisAttributes(G4Colour(0,255,255));
  cyanBox->SetForceSolid(true);
  actLV->SetVisAttributes(cyanBox);



      

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
