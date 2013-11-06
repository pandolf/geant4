//
// ********************************************************************
// This is description of Hadron Endcap calorimeter of CMS experiment
// with ECAL, SE and all dead material in front of 
// ********************************************************************
//

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Box.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4UniformMagField.hh"
#include "G4NistManager.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4ProductionCuts.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction():defaultMaterial(0),
 solidWorld(0),logicWorld(0),physiWorld(0),
 solidCalor(0),logicCalor(0),physiCalor(0),
 solidLayer(0),logicLayer(0),physiLayer(0),
 solidAbsorber(0),logicAbsorber(0),physiAbsorber(0),
 solidWrap(0),logicWrap(0),physiWrap(0),
 solidGap(0),logicGap(0),physiGap(0),
 solidSens(0),logicSens(0),physiSens(0),
 solWrapZero(0),logWrapZero(0),phyWrapZero(0),
 solGapZero(0),logGapZero(0),phyGapZero(0),
 solSensZero(0),logSensZero(0),phySensZero(0),
 solidCables(0),logicCables(0),physiCables(0),
 solidG10plate(0),logicG10plate(0),physiG10plate(0),
 solidEcal(0),logicEcal(0),physiEcal(0),
 solEcalLayer(0),logEcalLayer(0),phyEcalLayer(0),
 solEcalAbs(0),logEcalAbs(0),phyEcalAbs(0),
 solEcalSens(0),logEcalSens(0),phyEcalSens(0),
 solidSupport(0),logicSupport(0),physiSupport(0),
 solidAluSe(0),logicAluSe(0),physiAluSe(0),
 solidLeadSe(0),logicLeadSe(0),physiLeadSe(0),
 magField(0)
{

// default parameter values of the calorimeter, Hadron Endcap (HE)
//----------------------------------------------------------------
  AbsorberThickness =  79.0*mm;
  AirGapThickness   =   9.0*mm;
  WrapThickness     =   8.0*mm;
  GapThickness      =   6.0*mm;
  HcalSensThickness =   3.7*mm;
  NbOfHcalLayers    =  17;
  CalorSizeXY       = 540.0*cm;

  LayerThickness = AbsorberThickness + AirGapThickness;
  CalorThickness = NbOfHcalLayers*LayerThickness;

// parameters for zero scintillator layer in front of HE
//------------------------------------------------------
  ZeroWrapThick = 15.0*mm;
  ZeroGapThick  = 13.0*mm;
  ZeroSensThick =  9.0*mm;
  
// dead material in front of HE
//-----------------------------
  CablesThickness   = 23.40*cm;
  G10plateThickness = 2.5*mm;
  SuppThickness     = 4.5*mm;
  AluSeThickness    = 27.2*mm;
  LeadSeThickness   = 13.0*mm;

// Ecal default parameters
//-----------------------
  EcalAbsThickness  =   0.0*mm;
  EcalSensThickness = 220.0*mm;
  NbOfEcalLayers    = 1;
  offsetEcal        = 315.4*cm; //distance from origin to front of ECAL
  offsetHcal        = 81.8*cm; //distance from front of ECAL to front of HCAL
  ComputeEcalParameters();

// materials
//-----------
  DefineMaterials();
  SetHcalAbsMaterial("Brass_def");
  SetEcalAbsMaterial("Lead_def");
  SetEcalSensMaterial("PbWO_def");
  SetHcalSensMaterial("Sci_def");

// Total number and transverse size of Ecal cells 
// for selected area ------------------------------
//-------------------  
  NbOfEcalCells = 25;
  EcalCellSize  = 28.62*mm; 

// Birks constants for scintillator in Hcal (default)
//---------------------------------------------------
  HcalBirksConst[0] = 0.00;
  HcalBirksConst[1] = 0.00;
  HcalBirksConst[2] = 0.00;

// Birks constants for Ecal (default)
//------------------------------------
  EcalBirksConst[0] = 0.00;  
  EcalBirksConst[1] = 0.00; 
  EcalBirksConst[2] = 0.00;

// Birk L3 parametrization for Ecal (default)
//-------------------------------------------               
  EcalBirkL3Const[0] = 0.00;
  EcalBirkL3Const[1] = 0.00;
  EcalBirkL3Const[2] = 0.00;

// create commands for interactive definition of the calorimeter
//--------------------------------------------------------------
  detectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{delete detectorMessenger;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  return ConstructCalorimeter();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{ 
 //This function illustrates the possible ways to define materials
 
G4String symbol;             // a=mass of a mole;
G4double a, z, density;      // z=mean number of protons; 

G4int ncomponents, natoms;
G4double fractionmass;

//
// define Elements
//-----------------

G4Element* H  = new G4Element("Hydrogen",symbol="H" , z= 1., a= 1.01*g/mole);
G4Element* C  = new G4Element("Carbon"  ,symbol="C" , z= 6., a= 12.01*g/mole);
G4Element* N  = new G4Element("Nitrogen",symbol="N" , z= 7., a= 14.01*g/mole);
G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z= 8., a= 16.00*g/mole);
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

// define simple materials

pAlum =
new G4Material("Aluminium", z=13., a=26.98*g/mole, density=2.700*g/cm3);

//pLead =
//new G4Material("PbSe", z=82., a=207.20*g/mole, density=11.34*g/cm3);

pLead =
new G4Material("Lead", z=82., a=207.20*g/mole, density=11.34*g/cm3);

pLead_d =
new G4Material("Lead_def", z=82., a=207.20*g/mole, density=11.34*g/cm3);

pTung =
new G4Material("Tungsten", z=74., a=183.84*g/mole, density=19.25*g/cm3);

// define a material from elements.   case 1: chemical molecule
//--------------------------------------------------------------

// define scintillator (C_9H_10)_n (default)
//------------------------------------------
pSci_d = 
new G4Material("Sci_def", density= 1.032*g/cm3, ncomponents=2);
pSci_d->AddElement(C, natoms=9);
pSci_d->AddElement(H, natoms=10);

const G4int nScid = 1;
G4double eScid[nScid] = { 3.10*eV };
G4double rScid[nScid] = { 1.58    };

G4MaterialPropertiesTable* proScid = new G4MaterialPropertiesTable();
proScid->AddProperty("RINDEX", eScid, rScid, nScid);
pSci_d->SetMaterialPropertiesTable(proScid);

// define scintillator (C_9H_10)_n
//---------------------------------
pSci =
new G4Material("Scintillator", density= 1.032*g/cm3, ncomponents=2);
pSci->AddElement(C, natoms=9);
pSci->AddElement(H, natoms=10);

const G4int nSci = 1;
G4double eSci[nSci] = { 3.10*eV };
G4double rSci[nSci] = { 1.58    };
 
G4MaterialPropertiesTable* proSci = new G4MaterialPropertiesTable();
proSci->AddProperty("RINDEX", eSci, rSci, nSci);
pSci->SetMaterialPropertiesTable(proSci);

// define a material from elements.   case 2: mixture by fractional mass
//-----------------------------------------------------------------------

// define Air:
//------------
pAir = 
new G4Material("Air"  , density= 1.290*mg/cm3, ncomponents=2);
pAir->AddElement(N, fractionmass=0.7);
pAir->AddElement(O, fractionmass=0.3);

// define brass (70% Cu + 30% Zn):
//--------------------------------
pBra =
new G4Material("Brass"  , density= 8.530*g/cm3, ncomponents=2);
pBra->AddElement(Cu, fractionmass=0.7);
pBra->AddElement(Zn, fractionmass=0.3);

pBra_d =
new G4Material("Brass_def"  , density= 8.530*g/cm3, ncomponents=2);
pBra_d->AddElement(Cu, fractionmass=0.7);
pBra_d->AddElement(Zn, fractionmass=0.3);

// define G10 material (22% Si + 27% C + 42% O + 6% H + 3% Cl):
// ------------------------------------------------------------
pG10 =
new G4Material("G10plate", density= 1.70*g/cm3, ncomponents=5);
pG10->AddElement(Si, fractionmass=0.22);
pG10->AddElement(C , fractionmass=0.27);
pG10->AddElement(O , fractionmass=0.42);
pG10->AddElement(H , fractionmass=0.06);
pG10->AddElement(Cl, fractionmass=0.03);

// define cables material (58% Cu + 26% C + 14% O + 2% H):
// -------------------------------------------------------
pCab =
new G4Material("Cables" , density= 2.680*g/cm3, ncomponents=4);
pCab->AddElement(Cu, fractionmass=0.58);
pCab->AddElement(C , fractionmass=0.26);
pCab->AddElement(O , fractionmass=0.14);
pCab->AddElement(H , fractionmass=0.02);

// define ECAL crystals PWO4_def (45.5% PB + 40.4% W + 14.1% O):
// ---------------------------------------------------------
pPwo_d =
new G4Material("PbWO_def" , density= 8.300*g/cm3, ncomponents=3);
pPwo_d->AddElement(Pb, fractionmass=0.455);
pPwo_d->AddElement(W , fractionmass=0.404);
pPwo_d->AddElement(O , fractionmass=0.141);

const G4int nPwod = 1;
G4double ePwod[nPwod] = { 3.10*eV };
G4double rPwod[nPwod] = { 2.16    };

G4MaterialPropertiesTable* proPwod = new G4MaterialPropertiesTable();
proPwod->AddProperty("RINDEX", ePwod, rPwod, nPwod);
pPwo_d->SetMaterialPropertiesTable(proPwod);

// define ECAL crystals PWO4 (45.5% PB + 40.4% W + 14.1% O):
// ---------------------------------------------------------
pPwo =
new G4Material("PbWO" , density= 8.300*g/cm3, ncomponents=3);
pPwo->AddElement(Pb, fractionmass=0.455); 
pPwo->AddElement(W , fractionmass=0.404);
pPwo->AddElement(O , fractionmass=0.141);

const G4int nPwo = 1;
G4double ePwo[nPwo] = { 3.10*eV };
G4double rPwo[nPwo] = { 2.16    };

G4MaterialPropertiesTable* proPwo = new G4MaterialPropertiesTable();
proPwo->AddProperty("RINDEX", ePwo, rPwo, nPwo);
pPwo->SetMaterialPropertiesTable(proPwo);

// define ceramic scintillator LAG (Lu3Al5O12) 6.7 g/cm3
// ------------------------------------------------------
pSens1 =
new G4Material("LAG" , density= 6.700*g/cm3, ncomponents=3);
pSens1->AddElement(Lu, natoms=3 );
pSens1->AddElement(Al, natoms=5 );
pSens1->AddElement(O , natoms=12);

const G4int nLag = 1;
G4double eLag[nLag] = { 3.10*eV };
G4double rLag[nLag] = { 1.868   };
  
G4MaterialPropertiesTable* proLag = new G4MaterialPropertiesTable();
proLag->AddProperty("RINDEX", eLag, rLag, nLag);          
pSens1->SetMaterialPropertiesTable(proLag);

// define ceramic scintillator YAG (Y3Al5O12) 4.57 g/cm3
// ------------------------------------------------------
pSens2 =                
new G4Material("YAG" , density= 4.570*g/cm3, ncomponents=3);
pSens2->AddElement(Y , natoms=3 );
pSens2->AddElement(Al, natoms=5 );
pSens2->AddElement(O , natoms=12);

const G4int nYag = 1;
G4double eYag[nYag] = { 3.10*eV };
G4double rYag[nYag] = { 1.859   };   

G4MaterialPropertiesTable* proYag = new G4MaterialPropertiesTable();
proYag->AddProperty("RINDEX", eYag, rYag, nYag);
pSens2->SetMaterialPropertiesTable(proYag);

// define ceramic scintillator LSO (Lu2SiO5) 7.41 g/cm3
// ------------------------------------------------------  
pSens3 =
new G4Material("LSO" , density= 7.41*g/cm3, ncomponents=3);
pSens3->AddElement(Lu, natoms=2 );
pSens3->AddElement(Si, natoms=1 );
pSens3->AddElement(O , natoms=5 );

const G4int nLso = 1;
G4double eLso[nLso] = { 3.10*eV };
G4double rLso[nLso] = { 1.82    };  

G4MaterialPropertiesTable* proLso = new G4MaterialPropertiesTable();
proLso->AddProperty("RINDEX", eLso, rLso, nLso);
pSens3->SetMaterialPropertiesTable(proLso);

// define ceramic scintillator LYSO (Lu2(1-x)Y2xSiO5) (x=0.1) 7.11 g/cm3
// ----------------------------------------------------------------------  
pSens4 =
new G4Material("LYSO" , density= 7.30*g/cm3, ncomponents=4);
pSens4->AddElement(Lu, 71.45*perCent );
pSens4->AddElement(Y ,  4.03*perCent );
pSens4->AddElement(Si,  6.37*perCent );
pSens4->AddElement(O , 18.15*perCent );

const G4int nLyso = 1;
G4double eLyso[nLyso] = { 3.10*eV };
G4double rLyso[nLyso] = { 1.82    };

G4MaterialPropertiesTable* proLyso = new G4MaterialPropertiesTable();
proLyso->AddProperty("RINDEX", eLyso, rLyso, nLyso);
pSens4->SetMaterialPropertiesTable(proLyso);

// define ceramic scintillator LYSO:Ce (Lu2(1-x)Y2xSiO5)(Ce) (x=0.1) 7.11 g/cm3
// which doped by Ce ---------------------------------------------------------
// -------------------
pSens5 =
new G4Material("LYSOc" , density= 7.30*g/cm3, ncomponents=5);
pSens5->AddElement(Lu, 71.44*perCent );
pSens5->AddElement(Y ,  4.03*perCent );
pSens5->AddElement(Si,  6.37*perCent );
pSens5->AddElement(O , 18.14*perCent );
pSens5->AddElement(Ce,  0.02*perCent); 

const G4int nLysc = 1;
G4double eLysc[nLysc] = { 3.10*eV };
G4double rLysc[nLysc] = { 1.82    };

G4MaterialPropertiesTable* proLysc = new G4MaterialPropertiesTable();
proLysc->AddProperty("RINDEX", eLysc, rLysc, nLysc);
pSens5->SetMaterialPropertiesTable(proLysc);

// define ceramic scintillator YSO (Y2SiO5) 4.54 g/cm3
// ------------------------------------------------------
pSens6 =
new G4Material("YSO" , density= 4.54*g/cm3, ncomponents=3);  
pSens6->AddElement(Y , natoms=2 );
pSens6->AddElement(Si, natoms=1 );
pSens6->AddElement(O , natoms=5);

const G4int nYso = 1;
G4double eYso[nYso] = { 3.10*eV };
G4double rYso[nYso] = { 1.80    };

G4MaterialPropertiesTable* proYso = new G4MaterialPropertiesTable();
proYso->AddProperty("RINDEX", eYso, rYso, nYso);
pSens6->SetMaterialPropertiesTable(proYso);

// examples of vacuum

G4Material* Vacuum =
new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                           kStateGas, 2.73*kelvin, 3.e-18*pascal);

G4Material* beam = 
new G4Material("Beam", density= 1.e-5*g/cm3, ncomponents=1,
                       kStateGas, STP_Temperature, 2.e-2*bar);
beam->AddMaterial(pAir, fractionmass=1.);

G4cout << *(G4Material::GetMaterialTable()) << G4endl;

//default materials of the World

defaultMaterial  = Vacuum;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::ConstructCalorimeter()
{

// Clean old geometry, if any
//----------------------------
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

// complete the Calor parameters definition
//------------------------------------------
  ComputeEcalParameters();

// World
//=======

  solidWorld = new G4Box("World",                       //its name
                          300.0*cm, 300.0*cm, 600.0*cm);  //its size
                         
  logicWorld = new G4LogicalVolume(solidWorld,		//its solid
                                   defaultMaterial,	//its material
                                   "World");		//its name
                                   
  physiWorld = new G4PVPlacement(0,			//no rotation
  				 G4ThreeVector(),	//at (0,0,0)
                                 logicWorld,		//its logical volume				 
                                 "World",		//its name
                                 0,			//its mother  volume
                                 false,			//no boolean operation
                                 0);			//copy number
  
// Calorimeter, only air space (x = 0.0 cm - 149.6 cm )
//======================================================

  solidCalor=0; logicCalor=0; physiCalor=0;
  
  solidCalor = new G4Box("Calorimeter",		        //its name
    		   CalorSizeXY/2.,CalorSizeXY/2.,CalorThickness/2.); //size
    			     
  logicCalor = new G4LogicalVolume(solidCalor,  	//its solid
      			           defaultMaterial,	//its material
      		     	          "Calorimeter");	//its name
    				       
  physiCalor = new G4PVPlacement(0,			//no rotation
                   G4ThreeVector(0.0,0.0,offsetEcal + offsetHcal + CalorThickness/2.), //at (0,0,397.2+74.8)
                                 logicCalor,	        //its logical volume
                                "Calorimeter",	        //its name
                                 logicWorld,	        //its mother  volume
                                 false,		        //no boolean operation
                                 0);		        //copy number
  
// Layer, only air space (79.0*mm brass + 9.0*mm air gap = 88.0*mm)
//=================================================================

  solidLayer=0; logicLayer=0; physiLayer=0;

  solidLayer = new G4Box("Layer",			//its name
                   CalorSizeXY/2.,CalorSizeXY/2.,LayerThickness/2.); //size
                       
  logicLayer = new G4LogicalVolume(solidLayer,	//its solid
                                   pAir,	        //its material
                                  "Layer");	        //its name

  physiLayer = new G4PVReplica("Layer",		        //its name
    		                logicLayer,	        //its logical volume
      		                logicCalor,	        //its mother
                                kZAxis,		        //axis of replication
                                NbOfHcalLayers,	        //number of replica
                                LayerThickness);	//width of replica

// Replication may occur along:
//-----------------------------
// o Cartesian axes (kXAxis,kYAxis,kZAxis)
//
//   The replications, of specified width have coordinates of
//   form (-width*(nReplicas-1)*0.5+n*width,0,0) where n=0.. nReplicas-1
//   for the case of kXAxis, and are unrotated.
//
// o Radial axis (cylindrical polar) (kRho)
//
//   The replications are cons/tubs sections, centred on the origin
//   and are unrotated.
//   They have radii of width*n+offset to width*(n+1)+offset
//                      where n=0..nReplicas-1
//
// o Phi axis (cylindrical polar) (kPhi)
//   The replications are `phi sections' or wedges, and of cons/tubs form
//   They have phi of offset+n*width to offset+(n+1)*width where
//   n=0..nReplicas-1
  
// Absorber (79.0*mm brass inside layer)
//======================================

  solidAbsorber=0; logicAbsorber=0; physiAbsorber=0;  
  
  solidAbsorber = new G4Box("HcalAbs",   	                //its name
                      CalorSizeXY/2.,CalorSizeXY/2.,AbsorberThickness/2.); 
                          
  logicAbsorber = new G4LogicalVolume(solidAbsorber,                //its solid
      			              HcalAbsMaterial,             //its material
      			              HcalAbsMaterial->GetName()); //name
      			                  
  physiAbsorber = new G4PVPlacement(0,		                 //no rotation
        	      G4ThreeVector(0.,0.,-AirGapThickness/2.),  //its position
                                     logicAbsorber,              //logical volume
                                     HcalAbsMaterial->GetName(), //its name
                                     logicLayer,                 //its mother
                                     false,                      //no boulean operat
                                     0);                         //copy number

// Al Wrapper 8.0*mm for scintillator (1.0*mm wall thickness)
//===========================================================
                           
  solidWrap=0; logicWrap=0; physiWrap=0;
      
  solidWrap = new G4Box("Wrapper",
                         CalorSizeXY/2.,CalorSizeXY/2.,WrapThickness/2.);
  
  logicWrap = new G4LogicalVolume(solidWrap,
                                  pAlum,
                                 "Wrapper");
                                   
  physiWrap = new G4PVPlacement(0,                     //no rotation
                  G4ThreeVector(0.,0.,AbsorberThickness/2.), //its position
                                logicWrap,             //its logical volume$
                               "Wrapper",              //its name
                                logicLayer,            //its mother
                                false,                 //no boulean operat
                                0);                    //copy number

// Air gap 6.0*mm inside Al wrapper
//=================================
  
  solidGap=0; logicGap=0; physiGap=0;
                                  
  solidGap = new G4Box("Gap",
                        CalorSizeXY/2.,CalorSizeXY/2.,GapThickness/2.);
     
  logicGap = new G4LogicalVolume(solidGap,
                                 pAir,
                                "Gap");

  physiGap = new G4PVPlacement(0,                       //no rotation
                               G4ThreeVector(0.,0.,0.), //its position
                               logicGap,                //its logical volume
                              "Gap",                    //its name
                               logicWrap,               //its mother
                               false,                   //no boulean operat
                               0);                      //copy number
                                   
// Sensitive Scintillator 3.7*mm thickness (inside air gap)
//=========================================================

  solidSens=0; logicSens=0; physiSens=0;

  solidSens = new G4Box("Scintil",
                         CalorSizeXY/2.,CalorSizeXY/2.,HcalSensThickness/2.);

  logicSens = new G4LogicalVolume(solidSens,
                                  HcalSensMaterial,
                                  HcalSensMaterial->GetName());

  physiSens = new G4PVPlacement(0,                       //no rotation
                                G4ThreeVector(0.,0.,0.), //its position
                                logicSens,               //its logical volume$
                                HcalSensMaterial->GetName(), //its name
                                logicGap,                //its mother
                                false,                   //no boulean operat
                                0);                      //copy number

// Al Wrapper 15.0*mm for zero layer (1.0*mm wall thickness)
//===========================================================
                           
  solWrapZero=0; logWrapZero=0; phyWrapZero=0;
      
  solWrapZero = new G4Box("Zwrapper",
                           CalorSizeXY/2.,CalorSizeXY/2.,ZeroWrapThick/2.);
  
  logWrapZero = new G4LogicalVolume(solWrapZero,
                                    pAlum,
                                   "Zwrapper");
                                   
  phyWrapZero = new G4PVPlacement(0,                       //no rotation
                  G4ThreeVector(0.,0.,offsetEcal + offsetHcal - ZeroWrapThick/2.),  //position
                                 logWrapZero,              //logical volume
                                "Zwrapper",                //name
                                 logicWorld,               //mother
                                 false,                    //no boulean operat
                                 0);                       //copy number

// Air gap 13.0*mm inside Al wrapper for zero layer
//=================================================
  
  solGapZero=0; logGapZero=0; phyGapZero=0;
                                  
  solGapZero = new G4Box("ZeroGap",
                          CalorSizeXY/2.,CalorSizeXY/2.,ZeroGapThick/2.);
     
  logGapZero = new G4LogicalVolume(solGapZero,
                                 pAir,
                                "ZeroGap");

  phyGapZero = new G4PVPlacement(0,                       
                               G4ThreeVector(0.,0.,0.), 
                               logGapZero,               
                              "ZeroGap",                    
                               logWrapZero,              
                               false,                   
                               0);                      

// Scintillator 9.0*mm thickness of zero layer 
//=============================================

  solSensZero=0; logSensZero=0; phySensZero=0;

  solSensZero = new G4Box("ZeroScin",
                           CalorSizeXY/2.,CalorSizeXY/2.,ZeroSensThick/2.);

  logSensZero = new G4LogicalVolume(solSensZero,
                                    pSci,
                                   "ZeroScin");

  phySensZero = new G4PVPlacement(0,                       
                                  G4ThreeVector(0.,0.,0.), 
                                  logSensZero,               
                                 "ZeroScin",                
                                  logGapZero,                
                                  false,                  
                                  0);                      

// Cables 23.40*cm centered between ECAL and HCAL
//===============================================

  solidCables=0; logicCables=0; physiCables=0;
      
  solidCables = new G4Box("Cables",
                           CalorSizeXY/2.,CalorSizeXY/2.,CablesThickness/2.);
  
  logicCables = new G4LogicalVolume(solidCables,
                                    pCab,
                                   "Cables");
  
  //centered between ECAL G10 plate and zero layer
  middleCables = offsetEcal + EcalCalorThickness + G10plateThickness + 
				((offsetHcal - ZeroWrapThick) - (EcalCalorThickness + G10plateThickness))/2.;
								   
  physiCables = new G4PVPlacement(0,                     
                    G4ThreeVector(0.0, 0.0, middleCables), 
                                   logicCables,             
                                  "Cables",             
                                   logicWorld,            
                                   false,                 
                                   0);                   

// G10 plate 2.5*mm behind of ECAL and in front of HE (directly behind ECAL)
//==========================================================================

  solidG10plate=0; logicG10plate=0; physiG10plate=0;

  solidG10plate = new G4Box("G10plate",
                      CalorSizeXY/2.,CalorSizeXY/2.,G10plateThickness/2.);

  logicG10plate = new G4LogicalVolume(solidG10plate,
                                    pG10,
                                   "G10plate");
                                     
  physiG10plate = new G4PVPlacement(0,
                      G4ThreeVector(0.0, 0.0,offsetEcal + EcalCalorThickness + G10plateThickness/2.),
                                     logicG10plate,  
                                    "G10plate",
                                     logicWorld,
                                     false,
                                     0);

// Ecal calorimeter (total thickness must be <= 400.0 mm)
//------------------------------------------------------- 

  solidEcal=0; logicEcal=0; physiEcal=0;
  solEcalLayer=0; logEcalLayer=0; phyEcalLayer=0;
  middleEcal = offsetEcal + EcalCalorThickness/2.; 
  
  if (EcalCalorThickness > 0.)  
    { solidEcal = new G4Box("Ecal",                                  
                      CalorSizeXY/2.,CalorSizeXY/2.,EcalCalorThickness/2.);
                             
      logicEcal = new G4LogicalVolume(solidEcal,                 
                                      defaultMaterial,           
                                      "Ecal");                   
                                       
      physiEcal = new G4PVPlacement(0,                
//                                    G4ThreeVector(-70.8*cm, 0.0, 0.0),        
                                    G4ThreeVector(0.0, 0.0, middleEcal),        
                                    logicEcal,       
                                    "Ecal",          
                                    logicWorld,       
                                    false,            
                                    0);               

// Ecal layer
//------------
      solEcalLayer = new G4Box("EcLayer",                 
                     CalorSizeXY/2.,CalorSizeXY/2.,EcalLayerThickness/2.); 
                       
      logEcalLayer = new G4LogicalVolume(solEcalLayer,      
                                         defaultMaterial,
                                         "EcLayer");        
      if (NbOfEcalLayers > 1)                                      
        phyEcalLayer = new G4PVReplica("EcLayer",           
                                        logEcalLayer,        
                                        logicEcal,       
                                        kZAxis,            
                                        NbOfEcalLayers,        
                                        EcalLayerThickness);   
      else
        phyEcalLayer = new G4PVPlacement(0,               
                                         G4ThreeVector(),   
                                         logEcalLayer,                   
                                         "EcLayer",          
                                         logicEcal,       
                                         false,             
                                         0);                   
    }                                   
  
// Ecal absorber material
//-----------------------
  solEcalAbs=0; logEcalAbs=0; phyEcalAbs=0;  
  
  if (EcalAbsThickness > 0.) 
    { solEcalAbs = new G4Box("EcalAbs",            
                              CalorSizeXY/2.,CalorSizeXY/2.,EcalAbsThickness/2.); 
                          
      logEcalAbs = new G4LogicalVolume(solEcalAbs,    
                                       EcalAbsMaterial, 
                                       EcalAbsMaterial->GetName()); 
                                          
      phyEcalAbs = new G4PVPlacement(0,                 
                       G4ThreeVector(0.,0.,-EcalSensThickness/2.), 
                                      logEcalAbs,                 
                                      EcalAbsMaterial->GetName(), 
                                      logEcalLayer,       
                                      false,             
                                      0);                
    }
  
// Ecal sensitive material
//-------------------------
  solEcalSens=0; logEcalSens=0; phyEcalSens=0; 
  if (EcalSensThickness > 0.)
    { solEcalSens = new G4Box("EcalSens",
                        CalorSizeXY/2.,CalorSizeXY/2.,EcalSensThickness/2.);
                           
      logEcalSens = new G4LogicalVolume(solEcalSens,
                                        EcalSensMaterial,
                                        EcalSensMaterial->GetName());
                                     
      phyEcalSens = new G4PVPlacement(0,                     
                        G4ThreeVector(0.,0.,EcalAbsThickness/2.),  
                                      logEcalSens,                       
                                      EcalSensMaterial->GetName(), 
                                      logEcalLayer,             
                                      false,                  
                                      0);                     
    }

// Al support 4.5*mm in front of ECAL (at 3.2*cm in front of ECAL)
//=============================================================

  solidSupport=0; logicSupport=0; physiSupport=0;
    
  solidSupport = new G4Box("Support",
                            CalorSizeXY/2.,CalorSizeXY/2.,SuppThickness/2.);

  logicSupport = new G4LogicalVolume(solidSupport,
                                     pAlum,
                                    "Support");

  physiSupport = new G4PVPlacement(0,
                     G4ThreeVector(0.0, 0.0, offsetEcal - 3.2*cm),
                                    logicSupport,
                                   "Support",
                                    logicWorld,
                                    false,
                                    0);

// Aluminium part of SE 27.2*mm in front of Ecal (at -16.5*cm from ECAL front)
//=============================================================

  solidAluSe=0; logicAluSe=0; physiAluSe=0;

  solidAluSe = new G4Box("AlumSe",
                          CalorSizeXY/2.,CalorSizeXY/2.,AluSeThickness/2.);
                          
  logicAluSe = new G4LogicalVolume(solidAluSe,
                                   pAlum,
                                  "AlumSe");
                                  
  physiAluSe = new G4PVPlacement(0,
                   G4ThreeVector(0.0, 0.0, offsetEcal - 16.5*cm),
                                  logicAluSe,
                                 "AlumSe",   
                                  logicWorld,
                                  false,
                                  0);   

// Lead part of SE 1.3*cm in front of ECAL (at -18.75*cm from ECAL front)
//========================================================

  solidLeadSe=0; logicLeadSe=0; physiLeadSe=0;
                                
  solidLeadSe = new G4Box("LeadSe",
                           CalorSizeXY/2.,CalorSizeXY/2.,LeadSeThickness/2.);
                                
  logicLeadSe = new G4LogicalVolume(solidLeadSe,
                                    pLead,
                                   "LeadSe");

  physiLeadSe = new G4PVPlacement(0,
                    G4ThreeVector(0.0, 0.0, offsetEcal - 18.75*cm),
                                   logicLeadSe,
                                  "LeadSe",
                                   logicWorld,
                                   false,
                                   0);

// G10 plate 2.5*mm (copy) in front of SE (at -19.625*cm from ECAL front)
//========================================================
  physiG10plate = new G4PVPlacement(0,
                      G4ThreeVector(0.0, 0.0, offsetEcal - 19.625*cm),
                                     logicG10plate,
                                    "G10plate",
                                     logicWorld,
                                     false,
                                     1);								   

  PrintCalorParameters();     

// example how to change cuts in different medias through the input cards
//========================================================================

// change cuts in Ecal sensitive material
//---------------------------------------
   if( EcalSensThickness > 0. ) {
     if( EcalSensMaterial->GetName() != "PbWO_def" ) {
       G4Region* aRegion = new G4Region(EcalSensMaterial->GetName());
       logEcalSens->SetRegion(aRegion);
       aRegion->AddRootLogicalVolume(logEcalSens);
       G4ProductionCuts* cut1 = new G4ProductionCuts;
       cut1->SetProductionCut(0.1*mm);
       aRegion->SetProductionCuts(cut1);
     }
   }

// change cuts in Ecal absorption material
//----------------------------------------
   if( EcalAbsThickness > 0. ) {
     if( EcalAbsMaterial->GetName() == EcalSensMaterial->GetName() )
       { G4cout << "\n ===> Stop from DetectorConstruction::G4Region(): " 
             << "\n EcalAbsMaterial = " << EcalAbsMaterial->GetName()
             << " must not be the same as"
             << " EcalSensMaterial = " << EcalSensMaterial->GetName()
             << "\n Please check name of  EcalAbsMaterial"
             << G4endl;
         exit(1);
       }
          
     G4Region* bRegion = new G4Region(EcalAbsMaterial->GetName());
     logEcalAbs->SetRegion(bRegion);
     bRegion->AddRootLogicalVolume(logEcalAbs);
     G4ProductionCuts* cut2 = new G4ProductionCuts;
     cut2->SetProductionCut(0.1*mm);
     bRegion->SetProductionCuts(cut2);
   }

// change cuts in Hcal absorption material
//----------------------------------------
   if( HcalAbsMaterial->GetName() != "Brass_def") {
     G4Region* cRegion = new G4Region(HcalAbsMaterial->GetName());
     logicAbsorber->SetRegion(cRegion);
     cRegion->AddRootLogicalVolume(logicAbsorber);
     G4ProductionCuts* cut3 = new G4ProductionCuts;
     cut3->SetProductionCut(0.2*mm);
     cRegion->SetProductionCuts(cut3);   
   }

//                                        
// Visualization attributes
//
  logicWorld->SetVisAttributes (G4VisAttributes::Invisible);

//  G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
//  simpleBoxVisAtt->SetVisibility(true);
//  logicCalor->SetVisAttributes(simpleBoxVisAtt);

//
// Below are vis attributes that permits someone to test / play 
// with the interactive expansion / contraction geometry system of the
// vis/OpenInventor driver :

// {G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,0.0));
//  simpleBoxVisAtt->SetVisibility(true);
//  delete logicCalor->GetVisAttributes();
//  logicCalor->SetVisAttributes(simpleBoxVisAtt);}


// {G4VisAttributes* atb= new G4VisAttributes(G4Colour(0.5,1.0,1.0));
//  atb->SetForceSolid(true);
//  logicLayer->SetVisAttributes(atb);}
//  logicAbsorber->SetVisAttributes(atb);}

 {G4VisAttributes* atb= new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  atb->SetForceSolid(true);
//  logicWrap->SetVisAttributes(atb);}
  logicGap->SetVisAttributes(atb);}
//  logicSens->SetVisAttributes(atb);}
  
 {G4VisAttributes* atb= new G4VisAttributes(G4Colour(0.0,1.0,0.0));
  atb->SetForceSolid(true);
  logSensZero->SetVisAttributes(atb);}

 {G4VisAttributes* atb= new G4VisAttributes(G4Colour(1.0,0.0,1.0));
  atb->SetForceSolid(true);
  logicCables->SetVisAttributes(atb);}

 {G4VisAttributes* atb= new G4VisAttributes(G4Colour(0.5,1.0,1.0));
  atb->SetForceSolid(true);
  logicG10plate->SetVisAttributes(atb);}

 {G4VisAttributes* atb= new G4VisAttributes(G4Colour(1.0,1.0,0.5));
  atb->SetForceSolid(true);  
  logEcalSens->SetVisAttributes(atb);} 

 {G4VisAttributes* atb= new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  atb->SetForceSolid(true);  
  logicSupport->SetVisAttributes(atb);} 

 {G4VisAttributes* atb= new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  atb->SetForceSolid(true);
  logicAluSe->SetVisAttributes(atb);}

 {G4VisAttributes* atb= new G4VisAttributes(G4Colour(1.0,0.0,0.0));
  atb->SetForceSolid(true);
  logicLeadSe->SetVisAttributes(atb);}

// {//Set opacity = 0.2 then transparency = 1 - 0.2 = 0.8
//  G4VisAttributes* atb= new G4VisAttributes(G4Colour(0.0,0.0,1.0,0.2));
//  atb->SetForceSolid(true);
//  logicGap->SetVisAttributes(atb);}

// {//Set opacity = 0.2 then transparency = 1 - 0.2 = 0.8
//  G4VisAttributes* atb= new G4VisAttributes(G4Colour(0.0,0.0,1.0,1.0));
//  atb->SetForceSolid(true);
//  logicGap->SetVisAttributes(atb);}

// {G4VisAttributes* atb= new G4VisAttributes(G4Colour(1.0,0.0,0.0));
//  atb->SetForceSolid(true);
//  logicWrap->SetVisAttributes(atb);}

//
// always return the physical World
//
  return physiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::PrintCalorParameters()
{
  G4cout << "\n------------------------------------------------------------"
         << "\n---> The HCAL calorimeter is " << NbOfHcalLayers 
         << " layers of:"
         << "\n---> [ "
         << AbsorberThickness/mm << "mm of " << HcalAbsMaterial->GetName() 
         << " + "
         << HcalSensThickness/mm << "mm of " << HcalSensMaterial->GetName()  
         << " ] " 
         << "\n------------------------------------------------------------\n";

  G4cout << "\n------------------------------------------------------------"
         << "\n---> The ECAL calorimeter is " << NbOfEcalLayers
         << " layers of:"
         << "\n---> [ "
         << EcalAbsThickness/mm << "mm of " << EcalAbsMaterial->GetName()
         << " + "
         << EcalSensThickness/mm << "mm of " << EcalSensMaterial->GetName()  
         << " ] "
         << "\n------------------------------------------------------------\n";
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// search the material by its name
//---------------------------------
void DetectorConstruction::SetEcalAbsMaterial(G4String materialChoice)
{
  G4Material* pttoMaterial = G4Material::GetMaterial(materialChoice);     
  if (pttoMaterial) EcalAbsMaterial = pttoMaterial;
}

void DetectorConstruction::SetHcalAbsMaterial(G4String materialChoice)
{
  G4Material* pttoMaterial = G4Material::GetMaterial(materialChoice);
  if (pttoMaterial) HcalAbsMaterial = pttoMaterial;

  if(HcalAbsMaterial->GetName() != "Brass_def" && HcalAbsMaterial->GetName() != "Brass")  
    { G4cout << "\n ===> Stop from DetectorConstruction::SetHcalAbsMaterial(): "
              << "\n HcalAbsMaterial = " << HcalAbsMaterial->GetName()  
              << "\n Brass is only possible for the Hcal "
              << "absorption material at this moment."
              << G4endl;
       exit(1);
    }
}

// change Ecal absorber thickness and recompute the calorimeter parameters
//-------------------------------------------------------------------------
void DetectorConstruction::SetEcalAbsThickness(G4double val)
{
  EcalAbsThickness = val;
  if (EcalAbsThickness > 400.000)
    { G4cout << "\n ===> Stop from DetectorConstruction::SetEcalAbsThickness(): " 
             << "\n EcalAbsThickness = " << EcalAbsThickness  
             << " mm, greater than 400.0 mm. "
             << "\n Check value of EcalAbsThickness." 
             << G4endl;
      exit(1);
    }
  if (EcalAbsThickness > 220.001)
    { G4cout << "\n ===> Warning from DetectorConstruction::SetEcalAbsThickness(): "
             << "\n EcalAbsThickness = " << EcalAbsThickness
             << " mm, greater than 220.0 mm. "
             << "\n Are you shure that it is correct ? "
             << G4endl;
    }
}

// search the material by its name
//--------------------------------
void DetectorConstruction::SetEcalSensMaterial(G4String materialChoice)
{
  G4Material* pttoMaterial = G4Material::GetMaterial(materialChoice);
  if (pttoMaterial) EcalSensMaterial = pttoMaterial;

  if( EcalSensMaterial->GetName() != "PbWO_def" && EcalSensMaterial->GetName() != "LAG"
    && EcalSensMaterial->GetName() != "YAG"  && EcalSensMaterial->GetName() != "LSO"
    && EcalSensMaterial->GetName() != "YSO"  && EcalSensMaterial->GetName() != "LYSO"
    && EcalSensMaterial->GetName() != "PbWO" && EcalSensMaterial->GetName() != "LYSOc")
     { G4cout << "\n ===> Stop from DetectorConstruction::SetEcalSensMaterial(): "
              << "\n EcalSensMaterial = " <<  EcalSensMaterial->GetName()
              << "\n Please choose Ecal sensitive material from "
              << "this possible list:"
              << "\n PbWO, LAG, YAG, LSO, YSO, LYSO and LYSOc"
              << G4endl;
       exit(1);
     }
}

// change Ecal sensitive thickness and recompute the calorimeter parameters 
//--------------------------------------------------------------------------
void DetectorConstruction::SetEcalSensThickness(G4double val)
{
  EcalSensThickness = val;
  if (EcalSensThickness > 400.000)
    { G4cout << "\n ===> Stop from DetectorConstruction::SetEcalSensThickness(): " 
             << "\n EcalSensThickness = " << EcalSensThickness  
             << " mm, greater than 220.0 mm "
             << "\n Check value of EcalSensThickness. "  
             << G4endl;
      exit(1);
    }
  if (EcalSensThickness > 220.000)
    { G4cout << "\n ===> Warning from DetectorConstruction::SetEcalSensThickness(): "
             << "\n EcalSensThickness = " << EcalSensThickness
             << " mm, greater than 220.0 mm "
             << "\n Are you shure that it is correct ? "
             << G4endl;
    }
}

// Set Hcal sensitive material
//----------------------------
void DetectorConstruction::SetHcalSensMaterial(G4String materialChoice)
{
  G4Material* pttoMaterial = G4Material::GetMaterial(materialChoice);
  if (pttoMaterial) HcalSensMaterial = pttoMaterial;
     
  if( HcalSensMaterial->GetName() != "Sci_def" && HcalSensMaterial->GetName() != "LAG"
    && HcalSensMaterial->GetName() != "YAG"  && HcalSensMaterial->GetName() != "LSO"
    && HcalSensMaterial->GetName() != "YSO"  && HcalSensMaterial->GetName() != "LYSO"
    && HcalSensMaterial->GetName() != "PbWO" && HcalSensMaterial->GetName() != "LYSOc")
     { G4cout << "\n ===> Stop from DetectorConstruction::SetHcalSensMaterial(): "
              << "\n HcalSensMaterial = " <<  HcalSensMaterial->GetName()
              << "\n Please choose Hcal sensitive material from "
              << "this possible list:"
              << "\n PbWO, LAG, YAG, LSO, YSO, LYSO and LYSOc"
              << G4endl;
       exit(1);
     }

  if( EcalSensMaterial->GetName() == HcalSensMaterial->GetName() )
    { G4cout << "\n ===> Stop from DetectorConstruction::SetHcalSensMaterial(): "
             << "\n HcalSensMaterial = " <<  HcalSensMaterial->GetName()
             << " is equal to " 
             << "EcalSensMaterial = " <<  EcalSensMaterial->GetName()
             << "\n Please choose another Hcal sensitive material from "
             << "this possible list:"
             << "\n PbWO, LAG, YAG, LSO, YSO, LYSO and LYSOc"
             << G4endl;
      exit(1);
    }

}


// change Number of Ecal layers and recompute the calorimeter parameters
//-----------------------------------------------------------------------
void DetectorConstruction::SetNbOfEcalLayers(G4int val)
{
  NbOfEcalLayers = val;
  if (NbOfEcalLayers <= 0)
    { G4cout << "\n ===> Stop from DetectorConstruction::SetNbOfEcalLayers(): " 
             << "\n NbOfEcalLayers = " << NbOfEcalLayers  
             << " less than 1. "
             << "\n Check value of NbOfEcalLayers. " 
             << G4endl;
      exit(1);
    }
}

// Set number and size of Ecal cells
//-----------------------------------
  void DetectorConstruction::SetEcalCells(G4ThreeVector Value)
  {
    NbOfEcalCells = (G4int)Value(0);
    EcalCellSize  = (G4double)Value(1)*mm;
    if( NbOfEcalCells !=1 && NbOfEcalCells !=4  && NbOfEcalCells !=9
                          && NbOfEcalCells !=16 && NbOfEcalCells !=25 ) 
    { 
       G4cout << "\n ===> Stop from DetectorConstruction::SetEcalCells(): "
              << "\n NbOfEcalCells = " << NbOfEcalCells
              << " this number of cells is not allowed."
              << "\n Please check number NbOfEcalCells."
              << G4endl;
       exit(1);
    }

    if( EcalCellSize*NbOfEcalCells > CalorSizeXY*mm ) {
       G4cout << "\n ===> Stop from DetectorConstruction::SetEcalCells(): "
              << "\n EcalCellSize*NbOfEcalCells = " << EcalCellSize*NbOfEcalCells
              << " mm, greater "
              << " CalorSizeXY = " << CalorSizeXY*mm
              << "\n Check values in setEcalCells."
              << G4endl;
       exit(1);
    }
  }

// change dead material thickness
//-------------------------------
void DetectorConstruction::SetDeadThickness(G4double val)
{
  CablesThickness = val;
  if (CablesThickness > 400.0)
    { G4cout << "\n ===> Stop from DetectorConstruction::SetDeadThickness(): " 
             << "\n CablesThickness = " << CablesThickness  
             << " mm, greater than 400.0 mm "
             << "\n Check value of CablesThickness. "  
             << G4endl;
      exit(1);
    }
}  
  
// Set Birks constant for Hcal sensitive material
//------------------------------------------------
  void DetectorConstruction::SetHcalBirksConstant(G4ThreeVector Value)
  {
    HcalBirksConst[0] = G4double(Value(0));
    HcalBirksConst[1] = G4double(Value(1));
    HcalBirksConst[2] = G4double(Value(2));
  }

// Set Birks constant for Ecal sensitive material
//------------------------------------------------
  void DetectorConstruction::SetEcalBirksConstant(G4ThreeVector Value)
  {
    EcalBirksConst[0] = G4double(Value(0));
    EcalBirksConst[1] = G4double(Value(1));
    EcalBirksConst[2] = G4double(Value(2));
  }

// Set Birk L3 constant for Ecal sensitive material
//-------------------------------------------------
  void DetectorConstruction::SetEcalBirkL3Constant(G4ThreeVector Value)
  {
    EcalBirkL3Const[0] = G4double(Value(0));
    EcalBirkL3Const[1] = G4double(Value(1));
    EcalBirkL3Const[2] = G4double(Value(2));
  }

// Set magnetic field
//--------------------

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"

void DetectorConstruction::SetMagField(G4double fieldValue)
{
//apply a global uniform magnetic field along X axis

  G4FieldManager* fieldMgr
   = G4TransportationManager::GetTransportationManager()->GetFieldManager();

  if(magField) delete magField;         //delete the existing magn field

  if(fieldValue!=0.)                    // create a new one if non nul
  { magField = new G4UniformMagField(G4ThreeVector(0., 0., fieldValue));
    fieldMgr->SetDetectorField(magField);
    fieldMgr->CreateChordFinder(magField);
  } else {
    magField = 0;
    fieldMgr->SetDetectorField(magField);
  }

   G4cout << "\n---------------------------------------------------------------"
          << "\n===> You chose uniform magnetic field Value = "
          << fieldValue << " [kG] along Z-axis "  
          << "\n---------------------------------------------------------------\n";
}

// Update geometry
//-----------------

#include "G4RunManager.hh"
void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructCalorimeter());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
