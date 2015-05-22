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

#include "G4SubtractionSolid.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace CLHEP;

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

  G4Element* F  = new G4Element("Fluor" ,symbol="F" , z=9.,  a=18.998*g/mole);
  G4Element* Ce = new G4Element("Cerium",symbol="Ce", z=58., a=140.11*g/mole);

  G4Element* B  = new G4Element("Boron"  ,symbol="B" , z=5. , a=10.81*g/mole);
  G4Element* Ge = new G4Element("Germanium" ,symbol="Ge", z=32., a=72.64*g/mole);
  G4Element* O  = new G4Element("Oxygen" ,symbol="O" , z=8. , a=16.00*g/mole);

  G4Element* N  = new G4Element("Nitrogen" ,symbol="N" , z=7., a= 14.01*g/mole);

  G4Element* C  = new G4Element("Carbon" ,symbol="C" , z=6. , a=12.0107*g/mole);
  G4Element* H  = new G4Element("Hydrogen" ,symbol="H" , z=1. , a=1.00794*g/mole);

  G4Element* Si  = new G4Element("Silicon" ,symbol="Si" , z=14. , a=28.085*g/mole);

  G4Element* K = new G4Element("Potassium", symbol="K", z=19. , a=39.098*g/mole);
  G4Element* Cs = new G4Element("Caesium", symbol="Cs", z=55. , a=132.905*g/mole);
  G4Element* Sb  = new G4Element("Antimony" ,symbol="Sb" , z=51. , a=121.760*g/mole);


  //PMT Hamamatsu Bialkali Cathode
  //NO idea what the density is, nor the exact composition
  //but it is also not thaaaat imporant here
  //for density take density of tungstens
  G4Material* bialkali = new G4Material("Bialkali", density = 19.25*g/cm3, ncomponents = 3);
  bialkali->AddElement(K,2);
  bialkali->AddElement(Cs,1);
  bialkali->AddElement(Sb,1);

  //PMT Glass Window
  //density between 2.15 - 2.25 g/cm3
  //composition: 81% SiO2, 13% B2O3, (rest is other stuff)
  G4Material* boroslicate = new G4Material("Boroslicate", density = 2.23*g/cm3, ncomponents = 3);
  boroslicate->AddElement(O,fractionmass = 0.377);
  boroslicate->AddElement(Si,fractionmass = 0.362);
  boroslicate->AddElement(B,fractionmass = 0.261);
  
  G4Material* pAir = new G4Material("Air" , density= 1.290*mg/cm3, ncomponents=2);
  pAir->AddElement(N, fractionmass=0.7);
  pAir->AddElement(O, fractionmass=0.3);

  //optical grease
  G4Material* grease = new G4Material("Grease", density=1.0*g/cm3,3);
  grease->AddElement(C,1);
  grease->AddElement(H,1);
  grease->AddElement(O,1);
  
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

  //For Tyvek, first try, high density polyethylene (HDPE)
  G4Material* HDPE = new G4Material("HDPE", density=0.95*g/cm3, ncomponents=2);
  HDPE->AddElement(C , natoms=2);
  HDPE->AddElement(H , natoms=4);  

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole, density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);


  //Optical properties
  //Air
  const G4int nEntries_RI = 42;
  G4double PhotonEnergy_RI[nEntries_RI] =
    { 0.1000*eV, 1.0000*eV, 1.0121*eV, 1.0332*eV, 1.0552*eV, 1.0781*eV, 1.1021*eV, 1.1271*eV,
      1.1533*eV, 1.1808*eV, 1.2096*eV, 1.2398*eV, 1.2716*eV, 1.3051*eV, 1.3404*eV, 1.3776*eV,
      1.4170*eV, 1.4586*eV, 1.5028*eV, 1.5498*eV, 1.5998*eV, 1.6531*eV, 1.7101*eV, 1.7712*eV,
      1.8368*eV, 1.9074*eV, 1.9837*eV, 2.0664*eV, 2.1562*eV, 2.2543*eV, 2.3616*eV, 2.4797*eV,
      2.6102*eV, 2.7552*eV, 2.9173*eV, 3.0996*eV, 3.3062*eV, 3.5424*eV, 3.8149*eV, 4.1328*eV,
      4.5085*eV, 4.9594*eV };
  G4double RefractiveIndex[nEntries_RI] =
    { 1.0003, 1.0003, 1.0003, 1.0003,     1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003 };
  G4MaterialPropertiesTable* myMPT = new G4MaterialPropertiesTable();
  myMPT->AddProperty("RINDEX",PhotonEnergy_RI,RefractiveIndex,nEntries_RI);
  pAir->SetMaterialPropertiesTable(myMPT);

  //Polystyrene (Fibre core)//////////////////////////////////////////
  G4double photonEnergy[] =
    {0.9*eV, 2.00*eV,2.03*eV,2.06*eV,2.09*eV,2.12*eV,2.15*eV,2.18*eV,2.21*eV,2.24*eV,2.27*eV,
     2.30*eV,2.33*eV,2.36*eV,2.39*eV,2.42*eV,     2.45*eV,2.48*eV,2.51*eV,2.54*eV,2.57*eV,
     2.60*eV,2.63*eV,2.66*eV,2.69*eV,2.72*eV,    2.75*eV,2.78*eV,2.81*eV,2.84*eV,2.87*eV,
     2.90*eV,2.93*eV,2.96*eV,2.99*eV,3.02*eV,     3.05*eV,3.08*eV,3.11*eV,3.14*eV,3.17*eV,
     3.20*eV,3.23*eV,3.26*eV,3.29*eV,3.32*eV,     3.35*eV,3.38*eV,3.41*eV,3.44*eV,3.47*eV 
     , 5.5*eV };
  
  const G4int nEntries = sizeof(photonEnergy)/sizeof(G4double);

  G4double refractiveIndexPS[] =
    { 1.59,  1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59,
      1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59,
      1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59,
      1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59,
      1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59, 1.59};
  assert(sizeof(refractiveIndexPS) == sizeof(photonEnergy));
  
 

  //Polystyren absorption length (for different wavelengths)
  G4double photonEnergy_PS_abs[] ={
   1.77138 *eV, 1.77138 *eV, 1.77138 *eV, 1.77281 *eV, 1.77443 *eV, 1.77713 *eV, 1.7802 *eV,
   1.78473 *eV, 1.78965 *eV, 1.79551 *eV, 1.8016 *eV, 1.80978 *eV, 1.81635 *eV, 1.82315 *eV,
   1.82904 *eV, 1.83805 *eV, 1.84696 *eV, 1.85674 *eV, 1.86563 *eV, 1.87401 *eV, 1.88875 *eV,
   1.89836 *eV, 1.90911 *eV, 1.91516 *eV, 1.91914 *eV, 1.92335 *eV, 1.92907 *eV, 1.93674 *eV,
   1.94146 *eV, 1.94901 *eV, 1.95663 *eV, 1.9632 *eV, 1.97425 *eV, 1.98251 *eV, 1.98949 *eV,
   1.99719 *eV, 2.00267 *eV, 2.00863 *eV, 2.01209 *eV, 2.01533 *eV, 2.01904 *eV, 2.02161 *eV, 
   2.02371 *eV, 2.02581 *eV, 2.02815 *eV, 2.0298 *eV, 2.03097 *eV, 2.03262 *eV, 2.03427 *eV, 
   2.03569 *eV, 2.03853 *eV, 2.04138 *eV, 2.04328 *eV, 2.04567 *eV, 2.04782 *eV, 2.05093 *eV,
   2.05381 *eV, 2.05791 *eV, 2.06106 *eV, 2.06567 *eV, 2.07153 *eV, 2.08186 *eV, 2.0903 *eV, 
   2.10309 *eV, 2.11323 *eV, 2.12193 *eV, 2.14006 *eV, 2.15347 *eV, 2.16919 *eV, 2.17943 *eV, 
   2.19361 *eV, 2.22395 *eV, 2.24761 *eV, 2.2712 *eV, 2.28422 *eV, 2.29408 *eV, 2.30252 *eV, 
   2.31193 *eV, 2.31896 *eV, 2.32419 *eV, 2.32604 *eV, 2.33223 *eV, 2.34189 *eV, 2.34659 *eV,
   2.36049 *eV, 2.38717 *eV, 2.40287 *eV, 2.43356 *eV, 2.46435 *eV, 2.49486 *eV, 2.52322 *eV, 
   2.55299 *eV, 2.58004 *eV, 2.60071 *eV, 2.62957 *eV};
  
  G4double absPS[] ={ 
    3.6621*m, 4.52676*m, 4.69522*m, 4.96965*m, 5.29245*m, 5.80285*m, 6.3699*m, 7.3105*m, 
    8.14816*m, 9.40172*m, 10.657*m, 12.4558*m, 13.9683*m, 15.2778*m, 16.4333*m, 17.2296*m, 
    17.3058*m, 16.5026*m, 15.9637*m, 15.6445*m, 13.4403*m, 11.4696*m, 9.95195*m, 9.63329*m, 
    9.87656*m, 9.97734*m, 10.4296*m, 11.1428*m, 11.8879*m, 12.9937*m, 13.8202*m, 14.5395*m,
    15.3377*m, 15.2778*m, 14.8712*m, 14.3791*m, 13.6752*m, 12.7398*m, 11.4696*m, 10.2385*m, 
    8.76932*m, 7.40742*m, 6.72013*m, 6.00786*m, 5.43966*m, 5.05312*m, 4.82854*m, 4.67837*m, 
    4.52152*m, 4.41934*m, 4.36509*m, 4.35536*m, 4.42935*m, 4.62854*m, 4.85853*m, 5.11257*m, 
    5.52418*m, 5.96207*m, 6.57331*m, 7.33793*m, 8.44734*m, 10.0802*m, 11.2712*m, 12.2991*m, 
    12.4162*m, 12.7814*m, 12.6165*m, 11.9241*m, 11.2712*m, 10.6861*m, 10.5992*m, 10.0802*m, 
    9.70501*m, 9.20263*m, 8.7497*m, 8.46562*m, 8.08082*m, 7.40742*m, 6.81379*m, 6.55129*m, 
    6.41167*m, 6.2981*m, 6.33893*m, 6.47536*m, 6.60662*m, 6.59548*m, 6.4861*m, 6.1399*m, 
    5.75164*m, 5.3798*m, 4.96965*m, 4.64503*m, 4.26048*m, 3.95462*m, 3.6621*m };
  
  assert(sizeof(absPS) == sizeof(photonEnergy_PS_abs));

  const G4int nEntries_PS_abs = sizeof(photonEnergy_PS_abs)/sizeof(G4double);

  /*  
  G4double scintilFast[] =
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  assert(sizeof(scintilFast) == sizeof(photonEnergy));
  */

  G4double photonEnergy_emis[] = {
    1.90726 *eV, 1.92014 *eV, 1.9359 *eV, 1.95784 *eV, 1.97001 *eV, 1.98151 *eV, 1.99541 *eV,
    2.00202 *eV, 2.00951 *eV, 2.01621 *eV, 2.02466 *eV, 2.0321 *eV, 2.04089 *eV, 2.04737 *eV,
    2.05302 *eV, 2.05783 *eV, 2.0653 *eV, 2.07172 *eV, 2.07772 *eV, 2.08444 *eV, 2.0903 *eV, 
    2.09369 *eV, 2.10646 *eV, 2.12032 *eV, 2.13036 *eV, 2.1405 *eV, 2.15505 *eV, 2.17174 *eV, 
    2.18598 *eV, 2.1979 *eV, 2.20693 *eV, 2.214 *eV, 2.22189 *eV, 2.22829 *eV, 2.23524 *eV, 
    2.24224 *eV, 2.25216 *eV, 2.25821 *eV, 2.27253 *eV, 2.28273 *eV, 2.29274 *eV, 2.30393 *eV,
    2.31663 *eV, 2.32526 *eV, 2.33283 *eV, 2.33932 *eV, 2.3547 *eV, 2.36796 *eV, 2.38519 *eV,
    2.3994 *eV, 2.41558 *eV, 2.42436 *eV, 2.42832 *eV, 2.43627 *eV, 2.44489 *eV, 2.45389 *eV,
    2.46201 *eV, 2.47081 *eV, 2.47778 *eV, 2.48127 *eV, 2.49214 *eV, 2.50279 *eV, 2.51417 *eV,
    2.52566 *eV, 2.5336 *eV, 2.54964 *eV, 2.56485 *eV, 2.58369 *eV  };

  G4double emissionPS [] = {
    0.000740953, 0.0007897, 0.000896943, 0.00114068, 0.00132592, 0.0015209, 0.00184263, 
    0.00194013, 0.00194988, 0.00201812, 0.00213511, 0.00232035, 0.00256409, 0.00272983,
    0.00285657, 0.00296381, 0.0031978, 0.00349028, 0.00379251, 0.00410449, 0.00430923,
    0.00435797, 0.00472845, 0.00545965, 0.00590812, 0.00636634, 0.0071073, 0.00802374,
    0.00885244, 0.0094569, 0.00979812, 0.0101589, 0.0104611, 0.0107633, 0.0111143, 0.0113385, 
    0.0116603, 0.0119625, 0.0126937, 0.0132494, 0.0137954, 0.0142243, 0.0146436, 0.0148093,
    0.0148483, 0.0148093, 0.0140489, 0.0132494, 0.011982, 0.0102466, 0.00811148, 0.0070098, 
    0.00621035, 0.00540116, 0.00466995, 0.00382176, 0.00309055, 0.00224236, 0.00177439,
    0.00141366, 0.00116993, 0.000994437, 0.000750702, 0.000428973, 0.000263233, 0.000165739, 
    9.74938e-05, 4.87469e-05   };

  //  for(int k=0; k<sizeof(emissionPS);k++)
  //    emissionPS[k]= 100*emissionPS[k];  

  assert(sizeof(photonEnergy_emis) == sizeof(emissionPS));
  const G4int nEntries_emis = sizeof(photonEnergy_emis)/sizeof(G4double);

  G4double photonEnergy_WLS_abs[] = { 1*eV, 2*eV,3*eV, 6*eV};
  G4double WLS_abs[]= { 10*m, 4*m, 0.1*mm, 0.1*mm};
  const G4int nEntries_WLS_abs = sizeof(photonEnergy_WLS_abs)/sizeof(G4double);


  // Add entries into properties table
  G4MaterialPropertiesTable* mptPolystyrene = new G4MaterialPropertiesTable();
  mptPolystyrene->AddProperty("RINDEX",photonEnergy,refractiveIndexPS,nEntries);

  // mptPolystyrene->AddProperty("ABSLENGTH",photonEnergy_PS_abs,absPS,nEntries_PS_abs);

  mptPolystyrene->AddProperty("WLSABSLENGTH",photonEnergy_WLS_abs,WLS_abs,nEntries_WLS_abs);

  //  mptPolystyrene->AddProperty("WLSABSLENGTH",photonEnergy_PS_abs,absPS,nEntries_PS_abs);
  mptPolystyrene->AddConstProperty("SCINTILLATIONYIELD",10./keV);//10/keV nominally
  //  mptPolystyrene->AddConstProperty("RESOLUTIONSCALE",1.0);
  mptPolystyrene->AddProperty("WLSCOMPONENT",photonEnergy_emis,emissionPS,nEntries_emis);
  mptPolystyrene->AddConstProperty("WLSTIMECONSTANT", 7.0*ns);
  
  polystyrene->SetMaterialPropertiesTable(mptPolystyrene);
 
  // Set the Birks Constant for the Polystyrene scintillator
  // polystyrene->GetIonisation()->SetBirksConstant(0.126*mm/MeV);


  //PMMA
  G4double refractiveIndexWLSfiber[] =
    { 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
      1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
      1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
      1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
      1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49 , 1.49};
  assert(sizeof(refractiveIndexWLSfiber) == sizeof(photonEnergy));
  
  G4double absWLSfiber[] =  {
    20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m, 20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
    20.0*m,20.0*m,20.0*m,20.0*m,20.0*m, 20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
    20.0*m,20.0*m,20.0*m,20.0*m,20.0*m, 20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
    20.0*m,20.0*m,20.0*m,20.0*m,20.0*m, 20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
    20.0*m,20.0*m,20.0*m,20.0*m,20.0*m, 20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m };
  assert(sizeof(absWLSfiber) == sizeof(photonEnergy));
  /*
  G4double absWLSfiber[] =
    {5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,
     5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,
     5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,1.10*m,
     1.10*m,1.10*m,1.10*m,1.10*m,1.10*m,1.10*m, 1.*mm, 1.*mm, 1.*mm, 1.*mm,
     1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm};
  assert(sizeof(absWLSfiber) == sizeof(photonEnergy));
 
  G4double emissionFib[] =
    {0.05, 0.10, 0.30, 0.50, 0.75, 1.00, 1.50, 1.85, 2.30, 2.75,
     3.25, 3.80, 4.50, 5.20, 6.00, 7.00, 8.50, 9.50, 11.1, 12.4,
     12.9, 13.0, 12.8, 12.3, 11.1, 11.0, 12.0, 11.0, 17.0, 16.9,
     15.0, 9.00, 2.50, 1.00, 0.05, 0.00, 0.00, 0.00, 0.00, 0.00,
     0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00};
  assert(sizeof(emissionFib) == sizeof(photonEnergy));
   */
  // Add entries into properties table
  G4MaterialPropertiesTable* mptWLSfiber = new G4MaterialPropertiesTable();
  mptWLSfiber->AddProperty("RINDEX",photonEnergy,refractiveIndexWLSfiber,nEntries);
  mptWLSfiber->AddProperty("ABSLENGTH",photonEnergy,absWLSfiber,nEntries);
  //  mptWLSfiber->AddProperty("WLSABSLENGTH",photonEnergy,absWLSfiber,nEntries);
  //  mptWLSfiber->AddProperty("WLSCOMPONENT",photonEnergy,emissionFib,nEntries);
  //  mptWLSfiber->AddConstProperty("WLSTIMECONSTANT", 0.5*ns);
   PMMA->SetMaterialPropertiesTable(mptWLSfiber);




  //OPTICAL GREASE ///////////////////////////////////////////////////////////////
  const G4int nEntries_grease = 35;
  G4double PhotonEnergy_grease[nEntries_grease] =
    { 0.0001*eV, 1.000*eV, 2.034*eV, 2.068*eV,
      2.103*eV, 2.139*eV, 2.177*eV, 2.216*eV,
      2.256*eV, 2.298*eV, 2.341*eV, 2.386*eV,
      2.433*eV, 2.481*eV, 2.532*eV, 2.585*eV,
      2.640*eV, 2.697*eV, 2.757*eV, 2.820*eV,
      2.885*eV, 2.954*eV, 3.026*eV, 3.102*eV,
      3.181*eV, 3.265*eV, 3.353*eV, 3.446*eV,
      3.545*eV, 3.649*eV, 3.760*eV, 3.877*eV,
      4.002*eV, 4.136*eV, 6.260*eV };
  G4double RefractiveIndex_grease[nEntries_grease] =
    { 1.403, 1.403, 1.403, 1.403,      1.403, 1.403, 1.403, 1.403,
      1.403, 1.403, 1.403, 1.403,      1.403, 1.403, 1.403, 1.403,
      1.403, 1.403, 1.403, 1.403,      1.403, 1.403, 1.403, 1.403,
      1.403, 1.403, 1.403, 1.403,      1.403, 1.403, 1.403, 1.403,
      1.403, 1.403, 1.403 };
  G4MaterialPropertiesTable* mptGrease = new G4MaterialPropertiesTable();
  mptGrease->AddProperty("RINDEX", PhotonEnergy_grease, RefractiveIndex_grease, nEntries_grease);
  //myMPT->AddProperty("ABSLENGTH", PhotonEnergy, Absorption, nEntries);
  grease->SetMaterialPropertiesTable(mptGrease);




  //CeF3 = sens (sensitive material) //////////////////////////////////////////////////////
  G4double refractiveIndex_CeF3[] =
    { 1.68,   1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68,
      1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68,
      1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68,
      1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68,
      1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68, 1.68  , 1.68};
  assert(sizeof(refractiveIndex_CeF3) == sizeof(photonEnergy));
  

  const G4int nEntries_CeF = 178;
  G4double PhotonEnergy_CeF[nEntries_CeF] = {
    2.91782 * eV, 2.92101 * eV, 2.92332 * eV, 2.92465 * eV, 2.9267 * eV, 
    2.92831 * eV, 2.93107 * eV, 2.9342 * eV, 2.93707 * eV, 2.93887 * eV, 
    2.93959 * eV, 2.94004 * eV, 2.94094 * eV, 2.94247 * eV, 2.94409 * eV, 
    2.94634 * eV, 2.94924 * eV, 2.95232 * eV, 2.95386 * eV, 2.95631 * eV, 
    2.95886 * eV, 2.96177 * eV, 2.96406 * eV, 2.96643 * eV, 2.9701 * eV, 
    2.97249 * eV, 2.97672 * eV, 2.98051 * eV, 2.98384 * eV, 2.98727 * eV, 
    2.99089 * eV, 2.99546 * eV, 3.00079 * eV, 3.01888 * eV, 3.06098 * eV, 
    3.07527 * eV, 3.08801 * eV, 3.10356 * eV, 3.13095 * eV, 3.14211 * eV, 
    3.15345 * eV, 3.16091 * eV, 3.16998 * eV, 3.17899 * eV, 3.18478 * eV, 
    3.19187 * eV, 3.20431 * eV, 3.21589 * eV, 3.22528 * eV, 3.23428 * eV, 
    3.24224 * eV, 3.25014 * eV, 3.25498 * eV, 3.25895 * eV, 3.26293 * eV, 
    3.26659 * eV, 3.26981 * eV, 3.27371 * eV, 3.27773 * eV, 3.28276 * eV, 
    3.2868 * eV, 3.29029 * eV, 3.29356 * eV, 3.29706 * eV, 3.30102 * eV, 
    3.30488 * eV, 3.30863 * eV, 3.31399 * eV, 3.31972 * eV, 3.32385 * eV, 
    3.32914 * eV, 3.33284 * eV, 3.33758 * eV, 3.34234 * eV, 3.34734 * eV, 
    3.35213 * eV, 3.35821 * eV, 3.36385 * eV, 3.37282 * eV, 3.37887 * eV, 
    3.38744 * eV, 3.3921 * eV, 3.39678 * eV, 3.3993 * eV, 3.40447 * eV, 
    3.40894 * eV, 3.41245 * eV, 3.41742 * eV, 3.4218 * eV, 3.42656 * eV, 
    3.43108 * eV, 3.43537 * eV, 3.43967 * eV, 3.44497 * eV, 3.4493 * eV, 
    3.455 * eV, 3.4616 * eV, 3.46684 * eV, 3.47323 * eV, 3.47939 * eV, 
    3.4862 * eV, 3.48961 * eV, 3.49316 * eV, 3.49647 * eV, 3.4999 * eV, 
    3.50322 * eV, 3.50731 * eV, 3.51218 * eV, 3.51565 * eV, 3.51989 * eV, 
    3.52493 * eV, 3.52868 * eV, 3.53283 * eV, 3.53725 * eV, 3.54168 * eV, 
    3.54442 * eV, 3.54926 * eV, 3.55241 * eV, 3.55661 * eV, 3.56109 * eV, 
    3.56558 * eV, 3.57009 * eV, 3.5754 * eV, 3.58006 * eV, 3.58406 * eV, 
    3.58968 * eV, 3.59545 * eV, 3.60124 * eV, 3.60651 * eV, 3.61275 * eV, 
    3.6205 * eV, 3.62761 * eV, 3.63584 * eV, 3.64438 * eV, 3.65408 * eV, 
    3.66662 * eV, 3.67756 * eV, 3.6897 * eV, 3.7062 * eV, 3.71881 * eV,
    3.73267 * eV, 3.74722 * eV, 3.76012 * eV, 3.77133 * eV, 3.7832 * eV, 
    3.79485 * eV, 3.80446 * eV, 3.81806 * eV, 3.82992 * eV, 3.84125 * eV,
    3.85356 * eV, 3.86472 * eV, 3.87593 * eV, 3.88502 * eV, 3.89572 * eV,
    3.90427 * eV, 3.91891 * eV, 3.93141 * eV, 3.9414 * eV, 3.95274 * eV,
    3.96513 * eV, 3.97431 * eV, 3.98519 * eV, 3.99413 * eV, 4.00177 * eV, 
    4.00744 * eV, 4.0128 * eV, 4.01682 * eV, 4.01984 * eV, 4.02354 * eV, 
    4.02759 * eV, 4.03063 * eV, 4.03266 * eV, 4.03706 * eV, 4.04249 * eV, 
    4.04896 * eV, 4.05545 * eV, 4.06334 * eV };

  G4double Emission_CeF[nEntries_CeF] = {
    8.45186e-06, 0.000141569, 0.000245104, 0.000380334, 0.000547258, 0.000764893,
    0.00119382, 0.00170516, 0.00223974, 0.00259261, 0.00273206, 0.00290744,
    0.00304478, 0.00325185, 0.00342934, 0.0036829, 0.00387518, 0.00403154, 
    0.0041076, 0.00419001, 0.0042703, 0.00433792, 0.0043844, 0.00442666, 
    0.00447526, 0.00449639, 0.00453865, 0.00458513, 0.0046295, 0.00468444, 
    0.00474994, 0.00483869, 0.00493377, 0.00525283, 0.00598603, 0.00623113, 
    0.00645088, 0.0066981, 0.00712703, 0.00728973, 0.0074651, 0.00757075, 
    0.00770387, 0.00782431, 0.00789192, 0.00796165, 0.00808209, 0.00819619, 
    0.0082955, 0.00838424, 0.00846876, 0.00856173, 0.00859765, 0.00861244, 
    0.00862301, 0.00862089, 0.00859554, 0.00855962, 0.00853004, 0.0084751,
    0.00841805, 0.00838424, 0.008361, 0.00834198, 0.0083441, 0.00834832, 
    0.0083779, 0.008492, 0.00864625, 0.00880261, 0.00895686, 0.00907518, 
    0.00923365, 0.00939635, 0.00954215, 0.00964568, 0.0097302, 0.0097788, 
    0.0098443, 0.00988867, 0.00993938, 0.00996897, 0.0100155, 0.0100429, 
    0.0100936, 0.0101465, 0.0101951, 0.0102373, 0.0102965, 0.010343, 0.0104063,
    0.0104486, 0.0105014, 0.0105395, 0.0105733, 0.010569, 0.0105669, 0.0105669, 
    0.0105669, 0.0105733, 0.0105881, 0.0106134, 0.0106599, 0.0107212, 0.0107698,
    0.0108606, 0.0109536, 0.0110762, 0.0111649, 0.0112811, 0.011391, 0.0114966, 
    0.0115812, 0.0116593, 0.0117122, 0.0117502, 0.0117629, 0.0117671, 0.0117671, 
    0.0117586, 0.0117333, 0.0117164, 0.0116931, 0.0116678, 0.0116572, 0.0116424,
    0.0116614, 0.0116974, 0.0117312, 0.0117882, 0.0118601, 0.0119319, 0.012008, 
    0.0120756, 0.0121094, 0.0121136, 0.0120925, 0.0120502, 0.0119488, 0.0118432,
    0.0117206, 0.0115431, 0.0113741, 0.0112008, 0.0110276, 0.0108163, 0.0106641,
    0.0104021, 0.0101697, 0.00992037, 0.00964146, 0.00939635, 0.0091597, 
    0.00896953, 0.00875824, 0.00858075, 0.00827225, 0.00799757, 0.00777782,
    0.00751581, 0.00721155, 0.00696222, 0.00666218, 0.00632833, 0.00602829,
    0.0057367, 0.00533523, 0.00492532, 0.00458725, 0.003797, 0.00293068, 
    0.00213621, 0.00175587, 0.00116847, 0.00074165, 0.000424706, 0.00019228, 0  };
  assert(sizeof(Emission_CeF) == sizeof(PhotonEnergy_CeF));

 
 G4MaterialPropertiesTable* mptCeF = new G4MaterialPropertiesTable();
 mptCeF->AddProperty ("FASTCOMPONENT", PhotonEnergy_CeF, Emission_CeF, nEntries_CeF);
 mptCeF->AddProperty ("RINDEX", PhotonEnergy_RI, refractiveIndex_CeF3, nEntries_RI);
 // mptCeF->AddProperty ("RINDEX", PhotonEnergy_RI, refractiveIndex_CeF3, nEntries_RI)->SetSpline(true);
 //mptCeF->AddProperty ("ABSLENGTH", PhotonEnergy_ABS, Absorption, nEntries_ABS);
 
 mptCeF->AddConstProperty ("SCINTILLATIONYIELD", 100./MeV);//this should be 1000./MeV
 mptCeF->AddConstProperty ("RESOLUTIONSCALE", 1);
 // mptCeF->AddConstProperty ("RESOLUTIONSCALE", 2.58);
 mptCeF->AddConstProperty ("FASTTIMECONSTANT", 32.5 *ns);

 sens->SetMaterialPropertiesTable (mptCeF);





  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* EEShashDetectorConstruction::DefineVolumes()
{
  // Geometry parameters

  // Shashlik:
  //fNofLayers = 10;
  fNofLayers = 1;
  G4double absThickness = 3.1*mm;
  G4double actThickness = 10.*mm;
  G4double calorSizeXY  = 24.*mm;

  //  G4double layerThickness = absThickness + actThickness;

  G4double rotationDist = 357.*mm; //Because matrix doesn't start at box edge

  //Tyvek between layers
  G4double tyvekThickness = 0.2*mm;

  G4double layerThickness = absThickness + actThickness + 2.*tyvekThickness;
  G4double calorThickness = fNofLayers * layerThickness;
  fZtraslation += calorThickness/2.;

  //Tyvek around central channel
  G4double tyvekSizeXY=(24.0 +0.2+0.2)*mm;
  G4double tyvekLength = calorThickness;

  // BGO:
  fNofBGOs = 24;
  //G4double bgoLength = 50.*mm;
  G4double bgoLength = 240.*mm;
  //G4double bgoSizeXY = 25.*mm;


  // fibres:
  // G4double fibreLength = 50.*mm;
  G4double fibreLength = 420.*mm;

  // Weird plastic piece at beginning of shashlik, I shall name it PomPom
  G4double pompomSizeXY = 24 *mm;
  G4double pompomLength = 8*mm;

  //Optical Grease:
  G4double greaseThickness = 0.1 *mm;
  G4double greaseRadius = 1.*mm;

  //PMT Brosilicate Glass Window
  G4double borosilThickness = 1.*mm;
  G4double borosilRadius = 19.3 /2. *mm;

  //PMT Bialkali Cathode
  G4double cathodeThickness = 1.*mm;
  G4double cathodeRadius = 15. /2. *mm;


  //BEAMTELESCOPE STUFF:

  // Hodoscope 8x8x1mm:
  G4double hodoSizeXY = 8.5*mm;
  //G4double hodoSizeXY = 10.*mm;
  G4double hodoLength = 4.*mm;
  G4double hodoDistance = 30.*mm;

  // world:
  //G4double worldSizeXY = 20. * (5.*calorSizeXY);
  //G4double worldSizeZ  = 100. * (fibreLength); 
  G4double worldSizeXY = 5 * (3.*calorSizeXY);
  //  G4double worldSizeXY = 1.2 * (3.*calorSizeXY);
  G4double worldSizeZ = 5. * (fibreLength); 
  //  G4double worldSizeZ = 2. * (fibreLength); 
  
  // Get materials
  G4Material* defaultMaterial = G4Material::GetMaterial("Air");
  //G4Material* defaultMaterial = G4Material::GetMaterial("Galactic");
  //G4Material* absMaterial     = G4Material::GetMaterial("Lead");
  G4Material* absMaterial     = G4Material::GetMaterial("Tungsten");
  G4Material* actMaterial     = G4Material::GetMaterial("CeF3");
  G4Material* bgoMaterial     = G4Material::GetMaterial("BGO");
  G4Material* fibreCoreMaterial  = G4Material::GetMaterial("Polystyrene");
  G4Material* fibreCladMaterial  = G4Material::GetMaterial("PMMA");

  //  G4Material* scintMaterial = G4Material::GetMaterial("PMMA");
  G4Material* hodoMaterial    = G4Material::GetMaterial("PMMA");
  G4Material* pompomMaterial = G4Material::GetMaterial("POM");
  G4Material* tyvekMaterial = G4Material::GetMaterial("HDPE");
  G4Material* greaseMaterial = G4Material::GetMaterial("Grease");
  G4Material* PMTwindowMaterial = G4Material::GetMaterial("Boroslicate");
  G4Material* cathodeMaterial = G4Material::GetMaterial("Bialkali");


  
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
                 worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size
                         
  G4LogicalVolume* labLV
    = new G4LogicalVolume(
                 labS,           // its solid
                 defaultMaterial,  // its material
                 "lab");         // its name


  G4RotationMatrix* rotation = new G4RotationMatrix();
  rotation->rotateX(fRotation*3.14159265359/180.);

  G4RotationMatrix* rotationNeg = new G4RotationMatrix();
  rotationNeg->rotateX(-fRotation*3.14159265359/180.);
                                   
  G4VPhysicalVolume* labPV = new G4PVPlacement(
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
  G4double chamferLength = 2.0*mm;
  //  G4double chamferLength = 2.1*mm;
  G4double roundy = 0.05*mm; //to avoid the "sharp" 45° angle at the chamfers, as the photons get stuck too often there


  octagonFace.push_back( G4TwoVector(-calorSizeXY/2.                ,+calorSizeXY/2. - chamferLength - roundy*4) );// start from top left corner
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2. + roundy ,+calorSizeXY/2.  -chamferLength -roundy              ) );
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2. + roundy + chamferLength ,+calorSizeXY/2.  - roundy             ) );
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2. + chamferLength +  roundy*4,+calorSizeXY/2.                ) );
  //end of first chamfer//
  //second corner
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2. - chamferLength - roundy*4   ,+calorSizeXY/2.                ) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2. - chamferLength - roundy     ,+calorSizeXY/2.   -roundy             ) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2.  - roundy                    ,+calorSizeXY/2.   -roundy   - chamferLength  ) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2.   ,+calorSizeXY/2.   - chamferLength    - roundy*4    ) );


  octagonFace.push_back( G4TwoVector(+calorSizeXY/2.                ,-calorSizeXY/2. + chamferLength + roundy *4) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2. -roundy               ,-calorSizeXY/2. + chamferLength + roundy  ) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2. -roundy -chamferLength    ,-calorSizeXY/2. + roundy  ) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2.  -chamferLength - roundy*4    ,-calorSizeXY/2.   ) );


  octagonFace.push_back( G4TwoVector(-calorSizeXY/2. + chamferLength + roundy *4. ,-calorSizeXY/2.                ) );
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2. + chamferLength + roundy  ,-calorSizeXY/2. +roundy                ) );
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2. + roundy  ,-calorSizeXY/2. +roundy  + chamferLength           ) );
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2.,-calorSizeXY/2. + chamferLength   + roundy*4          ) );
 


  /*  octagonFace.push_back( G4TwoVector(-calorSizeXY/2.                ,+calorSizeXY/2. - chamferLength) );// start from top left corner
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2. + chamferLength,+calorSizeXY/2.                ) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2. - chamferLength,+calorSizeXY/2.                ) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2.                ,+calorSizeXY/2. - chamferLength) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2.                ,-calorSizeXY/2. + chamferLength) );
  octagonFace.push_back( G4TwoVector(+calorSizeXY/2. - chamferLength,-calorSizeXY/2.                ) );
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2. + chamferLength,-calorSizeXY/2.                ) );
  octagonFace.push_back( G4TwoVector(-calorSizeXY/2.                ,-calorSizeXY/2. + chamferLength) );
  */
  

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
                 G4ThreeVector(0., 0., -actThickness/2 -tyvekThickness), // its position
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
   
 
  
 G4VPhysicalVolume* ActPV = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., absThickness/2.), // its position
                 actLV,            // its logical volume                         
                 "ActPV",            // its name
                 layerLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 


  //                               
  // Tyvek Layer
  //
  G4VSolid* tyvekS 
    = new G4ExtrudedSolid( "Tyvek",            // its name
        octagonFace,
        tyvekThickness/2.,
        G4TwoVector(0.,0.), 1.,
        G4TwoVector(0.,0.), 1.);
  //= new G4Box("Tyvek",             // its name
  //             calorSizeXY/2, calorSizeXY/2, tyvekThickness/2); // its size
                         
  G4LogicalVolume* tyvekLV
    = new G4LogicalVolume(
                 tyvekS,             // its solid
                 tyvekMaterial,      // its material
                 "TyvekLV");         // its name
                                   
  G4VPhysicalVolume* TyvekLayerPV1 = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., actThickness/2.+absThickness/2. + tyvekThickness/2.), // its position
                 tyvekLV,            // its logical volume                         
                 "TyvekLayerPV1",            // its name
                 layerLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 


  G4VPhysicalVolume* TyvekLayerPV2 =   new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0.,  (absThickness-actThickness)/2. +tyvekThickness/2. -tyvekThickness), // its position
                 tyvekLV,            // its logical volume                         
                 "TyvekLayerPV2",            // its name
                 layerLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 


  //Optical Grease at end of fibre:

  G4VSolid* greaseS
    = new G4Tubs("Grease", 0, greaseRadius, greaseThickness/2., 0, twopi);  

  G4LogicalVolume* greaseLV
    = new G4LogicalVolume(
                 greaseS,     // its solid
                 greaseMaterial,  // its material
                 "GreaseLV");   // its name
               

  //PMT Borosilicate Glass window

  G4VSolid* borosilS
    = new G4Tubs("Borosil", 0, borosilRadius, borosilThickness/2., 0, twopi);  

  G4LogicalVolume* borosilLV
    = new G4LogicalVolume(
                 borosilS,     // its solid
                 PMTwindowMaterial,  // its material
                 "BorosilLV");   // its name


  //PMT Bialkali Cathode
  G4VSolid* cathodeS
    = new G4Tubs("Cathode", 0, cathodeRadius, cathodeThickness/2., 0, twopi);  

  G4LogicalVolume* cathodeLV
    = new G4LogicalVolume(
                 cathodeS,     // its solid
                 cathodeMaterial,  // its material
                 "CathodeLV");   // its name
    


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
 G4VPhysicalVolume* FibreCorePV =  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., 0.), // its position
                 fibreCoreLV,            // its logical volume                         
                 "FibreCorePV",            // its name
                 fibreLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 
  
 G4VPhysicalVolume* FibreCladPV =   new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., 0.), // its position
                 fibreCladLV,            // its logical volume                         
                 "FibreCladPV",            // its name
                 fibreLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 


  // and now place the 4 fibres:
  int fibreCopy=0;
  int greaseCopy=0;
  int borosilCopy=0;
  int cathodeCopy = 0;


  for( int ix=-1; ix<=1; ix+=2 ) {
    for( int iy=-1; iy<=1; iy+=2 ) {

      //sin and cos for the rotation
      G4double xPos = ix*(calorSizeXY/2.-0.696);
      G4double yPos = iy*(calorSizeXY/2.-0.696) + sin(fRotation*3.14159265359/180.)*sqrt(((fibreLength-calorThickness)/2.+calorThickness/2.)*((fibreLength-calorThickness)/2.+calorThickness/2) + xPos*xPos);


      G4VPhysicalVolume* FibrePV =  new G4PVPlacement(
						      rotation,                // no rotation
						      G4ThreeVector(xPos,yPos,  cos(-fRotation*3.14159265359/180.)*((fibreLength-calorThickness)/2.+calorThickness/2.)  - sin(fRotation*3.14159265359/180.)*(iy*(calorSizeXY/2.-0.696)) ), // its position
						      fibreLV,            // its logical volume  
						      "FibrePV",   // its name
						      labLV,          // its mother  volume
						      false,            // no boolean operation
						      fibreCopy,      // copy number
						      fCheckOverlaps);  // checking overlaps 

    
      G4double yPosGrease = iy*(calorSizeXY/2.-0.696) + sin(fRotation*3.14159265359/180.)*sqrt(((fibreLength-calorThickness)/2.+calorThickness/2.+ fibreLength/2. +greaseThickness/2.)*((fibreLength-calorThickness)/2.+calorThickness/2+ fibreLength/2. +greaseThickness/2.) + xPos*xPos) ;
      G4VPhysicalVolume* GreasePV =  new G4PVPlacement(
						       rotation,                // no rotation
						       G4ThreeVector(xPos,yPosGrease,  cos(-fRotation*3.14159265359/180.)*((fibreLength-calorThickness)/2.+calorThickness/2. + fibreLength/2. +greaseThickness/2.)  - sin(fRotation*3.14159265359/180.)*(iy*(calorSizeXY/2.-0.696))    ), // its position
						       greaseLV,            // its logical volume  
						       "Grease",            // its name
						       labLV,          // its mother  volume
						       false,            // no boolean operation
						       greaseCopy,                // copy number
						       fCheckOverlaps);  // checking overlaps 


      G4double yPosBoroSil = iy*(calorSizeXY/2.-0.696) + sin(fRotation*3.14159265359/180.)*sqrt(((fibreLength-calorThickness)/2.+calorThickness/2.+ fibreLength/2.+greaseThickness +borosilThickness/2.)*((fibreLength-calorThickness)/2.+calorThickness/2+ fibreLength/2.+greaseThickness +borosilThickness/2.) + xPos*xPos) ;
      new G4PVPlacement(
                     rotation,                // no rotation
                     G4ThreeVector(xPos,yPosBoroSil,  cos(-fRotation*3.14159265359/180.)*((fibreLength-calorThickness)/2.+calorThickness/2. + fibreLength/2.+greaseThickness +borosilThickness/2.)  - sin(fRotation*3.14159265359/180.)*(iy*(calorSizeXY/2.-0.696))    ), // its position
                     borosilLV,            // its logical volume                         
                     "BoroSil",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     borosilCopy,                // copy number
                     fCheckOverlaps);  // checking overlaps 


    G4double yPosCathode = iy*(calorSizeXY/2.-0.696) + sin(fRotation*3.14159265359/180.)*sqrt(((fibreLength-calorThickness)/2.+calorThickness/2.+ fibreLength/2.+greaseThickness+borosilThickness +cathodeThickness/2.)*((fibreLength-calorThickness)/2.+calorThickness/2+ fibreLength/2.+greaseThickness+borosilThickness +cathodeThickness/2.) + xPos*xPos) ;
      new G4PVPlacement(
                     rotation,                // no rotation
                     G4ThreeVector(xPos,yPosCathode,  cos(-fRotation*3.14159265359/180.)*((fibreLength-calorThickness)/2.+calorThickness/2. + fibreLength/2.+greaseThickness +borosilThickness+cathodeThickness/2.)  - sin(fRotation*3.14159265359/180.)*(iy*(calorSizeXY/2.-0.696))    ), // its position
                     cathodeLV,            // its logical volume                         
                     "Cathode",            // its name
                     labLV,          // its mother  volume
                     false,            // no boolean operation
                     cathodeCopy,                // copy number
                     fCheckOverlaps);  // checking overlaps 

      fibreCopy++;
      greaseCopy++;
      borosilCopy++;
      cathodeCopy++;
  


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
  // Tyvek around the central channel 
  //
  G4VSolid* TyvekCoverS
    = new G4Box( "TyvekCover",            // its name
		 tyvekSizeXY/2., tyvekSizeXY/2., tyvekLength/2.); //its size
  G4VSolid* TyvekSMinus
    = new G4Box( "TyvekMinus",            // its name
		 (tyvekSizeXY-0.4)/2.,  (tyvekSizeXY-0.4)/2., tyvekLength/2.); //its size
  //  G4VSolid* TyvekSMinus
  // = new G4Box( "TyvekMinus",            // its name
  //		 tyvekSizeXY/2.-0.1, tyvekSizeXY/2.-0.1, tyvekLength/2.); //its size

  G4VSolid* TyvekSubtacted = new G4SubtractionSolid("TyvekSub", TyvekCoverS, TyvekSMinus);
                   
  G4LogicalVolume* TyvekCoverLV 
     = new G4LogicalVolume(
                TyvekSubtacted,     // its solid
                tyvekMaterial,  // its material
                "TyvekCoverLV");   // its name
  

  G4VPhysicalVolume* TyvekCoverPV = new G4PVPlacement(
                     rotation,                // no rotation
                     G4ThreeVector(0., 0,  cos(-fRotation*3.14159265359/180.)*( tyvekLength/2.) ), // its position
                     TyvekCoverLV,            // its logical volume                         
                     "TyvekCoverPV",            // its name
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
  

  


  
  
  /////////////////////////////////
  ///////////SURFACEEEES////////////
  /////////////////////////////////
  
  
  //ACTIVE MATERIAL
  const G4int NUM = 2;
  
  G4double pp[NUM] = {1.5*eV, 5.1*eV};
  //  G4double rindex[NUM] = {1.59, 1.59};
  G4double reflectivity[NUM] = {0.5, 0.5};
  G4double reflectivity_Fib[NUM] = {0.9, 0.9};
  G4double efficiency[NUM] = {0.9 , 0.9};
  
  G4MaterialPropertiesTable *OpSurfacePropertyCef3 = new G4MaterialPropertiesTable();
  OpSurfacePropertyCef3 -> AddProperty("REFLECTIVITY",pp,reflectivity,NUM);
  //  OpSurfacePropertyCef3 -> AddProperty("EFFICIENCY",pp,efficiency,NUM);
  
  
  G4OpticalSurface* SurfCef3Tyvek = new G4OpticalSurface("SurfCef3Tyvek");
  
  SurfCef3Tyvek -> SetType(dielectric_LUT);
  SurfCef3Tyvek -> SetModel(LUT);
  SurfCef3Tyvek -> SetFinish(polishedtyvekair);
  SurfCef3Tyvek -> SetPolish( 0.02);
  SurfCef3Tyvek -> SetMaterialPropertiesTable(OpSurfacePropertyCef3);
  
  
  G4LogicalBorderSurface* CeF3TyvekLayer1Surf = 
    new G4LogicalBorderSurface("CeF3TyvekLayer1Surf", ActPV, TyvekLayerPV1, SurfCef3Tyvek);
  
  G4LogicalBorderSurface* CeF3TyvekLayer2Surf = 
    new G4LogicalBorderSurface("CeF3TyvekLayer2Surf", ActPV, TyvekLayerPV2, SurfCef3Tyvek);
  
  G4LogicalBorderSurface* CeF3TyvekCover = 
    new G4LogicalBorderSurface("CeF3TyvekCover", ActPV, TyvekCoverPV, SurfCef3Tyvek );
  
  
  
  //This is the champfer surface 
  G4OpticalSurface* SurfCef3Air = new G4OpticalSurface("SurfCef3Air");
  
  SurfCef3Air -> SetType(dielectric_dielectric); 
  SurfCef3Air -> SetModel(glisur);
  SurfCef3Air -> SetFinish(polished);
  SurfCef3Air -> SetPolish(0.4);   
  SurfCef3Air -> SetMaterialPropertiesTable(OpSurfacePropertyCef3);
  
  G4LogicalBorderSurface* CeF3Air = new G4LogicalBorderSurface("CeF3Air", ActPV , labPV,  SurfCef3Air);   
  
   
 
  G4MaterialPropertiesTable *fibre_surf_mt = new G4MaterialPropertiesTable();
  //fibre_surf_mt-> AddProperty("REFLECTIVITY",pp,reflectivity_Fib,NUM);
  // fibre_surf_mt-> AddProperty("EFFICIENCY",pp,efficiency,NUM);
  
  
  G4OpticalSurface* OpSurfaceFibre = new G4OpticalSurface("OpSurfaceFibre");
  
  OpSurfaceFibre -> SetType(dielectric_metal);
  OpSurfaceFibre -> SetFinish(polished);
  OpSurfaceFibre -> SetModel(glisur);
  OpSurfaceFibre -> SetPolish(1.0);
  
  // G4OpticalSurface* OpSurfaceFibre = new G4OpticalSurface("OpSurfaceFibre", glisur, polished, dielectric_metal);
  
  OpSurfaceFibre -> SetMaterialPropertiesTable(fibre_surf_mt);
  
  
  G4LogicalSkinSurface* SurfaceFibre = new G4LogicalSkinSurface("OpSurfaceFibre", fibreLV, OpSurfaceFibre);
 
 
  
      /*
      G4double ephoton[] = {7.0*eV, 7.14*eV};
      const G4int num = sizeof(ephoton)/sizeof(G4double);

      G4double fibreReflectivity[] = {1.0, 1.0};
      assert(sizeof(fibreReflectivity) == sizeof(ephoton));
      G4double fibreEfficiency[] = {0.0, 0.0};
      assert(sizeof(fibreEfficiency) == sizeof(ephoton));
      G4MaterialPropertiesTable* fibrePT = new G4MaterialPropertiesTable();
      fibrePT->AddProperty("REFLECTIVITY", ephoton, fibreReflectivity, num);
      fibrePT->AddProperty("EFFICIENCY", ephoton, fibreEfficiency, num);
      G4OpticalSurface* OpFibreSurface =
	new G4OpticalSurface("FibreSurface",unified,polished,dielectric_dielectric);
      OpFibreSurface->SetMaterialPropertiesTable(fibrePT);
      */

  //
  // print parameters
  //
  G4cout << "\n------------------------------------------------------------"
         << "\n---> The calorimeter is " << fNofLayers << " layers of: [ "
         << absThickness/mm << "mm of " << absMaterial->GetName() 
         << " + "
         << actThickness/mm << "mm of " << actMaterial->GetName() << " ] " 
         << "\n------------------------------------------------------------\n";

  /*


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
                     G4ThreeVector(0., 0., -scintXDistance-scintXLength/2. -rotationDist), //its position
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



  */


  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);
  fibreLV->SetVisAttributes (G4VisAttributes::Invisible);
  labLV->SetVisAttributes (G4VisAttributes::Invisible);


  G4VisAttributes* grayBox= new G4VisAttributes(G4Colour(0.5,0.5,0.5));
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
  //  pomBox->SetForceSolid(true);
  PompomLV->SetVisAttributes(pomBox);

  /*
  G4VisAttributes* violetBox= new G4VisAttributes(G4Colour(200,0,255));
  violetBox->SetForceSolid(true);
  ScintXLV->SetVisAttributes(violetBox);
  Scint21LV->SetVisAttributes(violetBox);
  Scint22LV->SetVisAttributes(violetBox);
  Scint23LV->SetVisAttributes(violetBox);
  Scint3LV->SetVisAttributes(violetBox);
   */

  G4VisAttributes* violet2Box= new G4VisAttributes(G4Colour(150,0,255));
  //  violet2Box->SetForceSolid(true);
  // Hodo11LV->SetVisAttributes(violet2Box);
  HodoLV->SetVisAttributes(violet2Box);
  //Hodo31LV->SetVisAttributes(violet2Box);



  G4VisAttributes* whiteBox= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  whiteBox->SetForceSolid(true);
  tyvekLV->SetVisAttributes(whiteBox);
  greaseLV->SetVisAttributes(whiteBox);

  G4VisAttributes* yellowBox= new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  yellowBox->SetForceSolid(true);
  borosilLV->SetVisAttributes(yellowBox);

  G4VisAttributes* blackBox= new G4VisAttributes(G4Colour(0.0,0.0,0.0));
  blackBox->SetForceSolid(true);
  cathodeLV->SetVisAttributes(blackBox);



  //  
  // BGO
  //  

  G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(0,0,0));
  simpleBoxVisAtt->SetVisibility(true);
  //matrixLV->SetVisAttributes(simpleBoxVisAtt);
  calorLV->SetVisAttributes(simpleBoxVisAtt);

  // create the matrix:

  int copyNumber = 0;
  int totalCopies = fNofBGOs;

  G4VSolid* bgoS 
     = new G4Box("BGOChannel",     // its name
                calorSizeXY/2, calorSizeXY/2, bgoLength/2); // its size
                         
  G4LogicalVolume* bgoLV 
     = new G4LogicalVolume(
                bgoS,     // its solid
                bgoMaterial,  // its material
                "BGOLV");   // its name

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
                     G4ThreeVector(xPos, yPos, cos(-fRotation*3.14159265359/180.)*(zPos + fZtraslation)  - sin(fRotation*3.14159265359/180.)*( iy*(calorSizeXY + miniGap))),  // at (0,0,0)
                     bgoLV,          // its logical volume                         
                     "BGOChannel",    // its name
                     labLV,          // its mother  volume
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

  /*
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

  */


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
