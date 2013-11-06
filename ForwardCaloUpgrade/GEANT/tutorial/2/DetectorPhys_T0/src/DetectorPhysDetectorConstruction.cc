// ********************************************************************
// *                                                                  *
// *         Experimentelle Methoden der Teilchenphysik               *
// *                                                                  *
// * Geant 4 Tutorials  . Detector physics                            *
// * RWTH Aachen 04.11.2010                                           *
// * Karim LAIHEM                                                     *
// ********************************************************************
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



#include "DetectorPhysDetectorConstruction.hh"

#include "G4Material.hh"

//***** Here don't forget to incude basic geometry classes : example #include "G4Box.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Sphere.hh"
#include "G4Orb.hh"
#include "G4Trd.hh"



//***** End of include basic geometry classes

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4ios.hh"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorPhysDetectorConstruction::DetectorPhysDetectorConstruction()
  :logicWorld(0),physiWorld(0),
   LogicalChamberGasBox(0),PhysicalChamberGasBox(0)
{;}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorPhysDetectorConstruction::~DetectorPhysDetectorConstruction()
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorPhysDetectorConstruction::Construct()
{
  DefineMaterials();
  return ConstructSetUp();
  
}

void DetectorPhysDetectorConstruction::DefineMaterials()
{ 


  //This function illustrates the possible ways to define materials
  
  G4String name;        
  G4double A, Z, density;           
  G4double temperature, pressure;


    // Vaccum material
  name        = "Vaccum";
  density     = universe_mean_density;
  pressure    = 3.e-18*pascal;
  temperature = 2.73*kelvin;
  Z=1.;
  A=1.01*g/mole;
  Vaccum = new G4Material(name, Z, A , density, kStateGas,temperature,pressure);

  
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorPhysDetectorConstruction::ConstructSetUp()
{

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  //     
  // World
  //

  G4double           WorldSizeX = 2 *m;
  G4double           WorldSizeY = 2 *m;
  G4double           WorldSizeZ = 2 *m;

  G4Box* solidWorld = new G4Box("World",			//its name
                   WorldSizeX/2,WorldSizeY/2,WorldSizeZ/2);	//its size

  logicWorld = new G4LogicalVolume(solidWorld,		        //its solid
                                   Vaccum,              //defaultMaterial,
                                   "World");		        //its name
                                   
  physiWorld = new G4PVPlacement(0,			        //no rotation
  				 G4ThreeVector(),	        //at (0,0,0)
                                 "World",		        //its name
                                 logicWorld,		        //its logical volume
                                 0,			        //its mother  volume
                                 false,			        //no boolean operation
                                 0);			        //copy number

 

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  //

  // Chamber Gas Box at (0,0,0)
  //

  G4double ChamberGasBoxX = 10 *cm;
  G4double ChamberGasBoxY = 10 *cm;
  G4double ChamberGasBoxZ = 10 *cm;







//OOOOOOOOOOOOOOOOOOOOOOoooooooooooooooo    Tubes	
G4double  Rmin   = 00. *cm;
G4double  Rmax   = 5. *cm;
G4double  Lc     = 10. *cm;
G4double  PhiMin1 = 0.  *deg;
G4double  PhiMax1 = 360.*deg;

 

 



//OOOOOOOOOOOOOOOOOOOOOOoooooooooooooooo Cone
G4double  Rmin1   = 0. *cm; //at -L 
G4double  Rmax1   = 5. *cm; //at -L 
G4double  Rmin2   = 0. *cm; //at +L 
G4double  Rmax2   = 2.5 *cm; //at +L 
G4double  L       = 10. *cm;
G4double  PhiMin  = 0.  *deg;
G4double  PhiMax  = 360.*deg;








//OOOOOOOOOOOOOOOOOOooooooooooooo Sphere
G4double  Rmin_s     = 0. *cm;  
G4double  Rmax_s     = 5. *cm;  
G4double  PhiMin_s   = 0. *deg; 
G4double  PhiMax_s   = 360. *deg;  
G4double  ThetaMin_s = 0. *deg;
G4double  ThetaMax_s = 180.  *deg;

 




//OOOOOOOOOOOOOOOOOOoooooooooooooooooooooo Solid sphere

 G4double RSphere = 5. *cm;
 






//OOOOOOOOOOOOOOOOOOoooooooooooooooooooooo Trapezoid

 G4double dx1 = 10*cm;
 G4double dx2 = 2.5*cm;
 G4double dy1 = 10*cm;
 G4double dy2 = 2.5*cm;
 G4double dz  = 10*cm;

 



//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOoooooooooooo Sphere solide

    // Visualization attributes

     G4VisAttributes * red         = new G4VisAttributes    ( G4Colour(255/255. ,0/255.   ,0/255.  ));
     G4VisAttributes * brown       = new G4VisAttributes    ( G4Colour(255/255. ,153/255. ,153/255.));
     G4VisAttributes * metal       = new G4VisAttributes    ( G4Colour(204/255. ,204/255. ,255/255.));
     G4VisAttributes * Lbleu       = new G4VisAttributes    ( G4Colour(0/255.   ,204/255. ,204/255.));
     G4VisAttributes * Lgreen      = new G4VisAttributes    ( G4Colour(153/255. ,255/255. ,153/255.));
     G4VisAttributes * yellow      = new G4VisAttributes( G4Colour(255/255. ,255/255. ,0/255. ));

   
    logicWorld->SetVisAttributes (G4VisAttributes::Invisible);
    LogicalChamberGasBox->SetVisAttributes (yellow);

 

    return physiWorld;
}
