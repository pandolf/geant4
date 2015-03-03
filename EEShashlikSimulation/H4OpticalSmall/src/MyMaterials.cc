#include "MyMaterials.hh"
#include "G4NistManager.hh"

using namespace CLHEP;

MyMaterials::MyMaterials()
{}


MyMaterials::~MyMaterials()
{}


G4Material* MyMaterials::Air()
{
  G4double a, z, density;
  G4int nelements;

  G4Element* O  = new G4Element("Oxygen"  ,"O" , z=8. , a=16.00*g/mole);
  G4Element* N  = new G4Element("Nitrogen" , "N" , z=7., a= 14.01*g/mole);


  G4Material* mat = new G4Material("Air" , density= 1.290*mg/cm3, nelements=2);
 mat->AddElement(N, 70.*perCent);
mat->AddElement(O, 30.*perCent);


  const G4int nEntries_RI = 42;
  G4double PhotonEnergy_RI[nEntries_RI] =
    { 0.1000*eV, 1.0000*eV, 1.0121*eV, 1.0332*eV,
      1.0552*eV, 1.0781*eV, 1.1021*eV, 1.1271*eV,
      1.1533*eV, 1.1808*eV, 1.2096*eV, 1.2398*eV,
      1.2716*eV, 1.3051*eV, 1.3404*eV, 1.3776*eV,
      1.4170*eV, 1.4586*eV, 1.5028*eV, 1.5498*eV,
      1.5998*eV, 1.6531*eV, 1.7101*eV, 1.7712*eV,
      1.8368*eV, 1.9074*eV, 1.9837*eV, 2.0664*eV,
      2.1562*eV, 2.2543*eV, 2.3616*eV, 2.4797*eV,
      2.6102*eV, 2.7552*eV, 2.9173*eV, 3.0996*eV,
      3.3062*eV, 3.5424*eV, 3.8149*eV, 4.1328*eV,
      4.5085*eV, 4.9594*eV };
  G4double RefractiveIndex[nEntries_RI] =
    { 1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003 };
  G4MaterialPropertiesTable* myMPT = new G4MaterialPropertiesTable();
  myMPT->AddProperty("RINDEX",PhotonEnergy_RI,RefractiveIndex,nEntries_RI);
  mat->SetMaterialPropertiesTable(myMPT);
  

  return mat;
  
}





G4Material* MyMaterials::Tungsten()
{
  /*
  G4NistManager* man = G4NistManager::Instance();
  G4Element* W = man->FindOrBuildElement("W");
  G4Element* Ni = man->FindOrBuildElement("Ni");
  G4Element* Cu = man->FindOrBuildElement("Cu");

  G4Material* mat = new G4Material("Tungsten",17.*g/cm3,3);
  mat->AddElement(W,90.*perCent);
  mat->AddElement(Ni,5.*perCent);
  mat->AddElement(Cu,5.*perCent);
  */

  G4double a, z, density;
  G4Material* mat =  new G4Material("Tungsten", z=74., a=183.84*g/mole, density=19.25*g/cm3);

  return mat;
}





G4Material* MyMaterials::OpticalGrease()
{
  G4double a, z, density;
  G4Element* H = new G4Element("Hydrogen", "H", z=1., a= 1.01*g/mole);
  G4Element* O = new G4Element("Oxygen" , "O", z=8., a=16.00*g/mole);
  G4Element* C = new G4Element("Carbon" , "C", z=6., a=12.01*g/mole);
  G4Material* mat = new G4Material("Grease", density=1.0*g/cm3,3);
  mat->AddElement(C,1);
  mat->AddElement(H,1);
  mat->AddElement(O,1);
  const G4int nEntries = 35;
  G4double PhotonEnergy[nEntries] =
    { 0.0001*eV, 1.000*eV, 2.034*eV, 2.068*eV,
      2.103*eV, 2.139*eV, 2.177*eV, 2.216*eV,
      2.256*eV, 2.298*eV, 2.341*eV, 2.386*eV,
      2.433*eV, 2.481*eV, 2.532*eV, 2.585*eV,
      2.640*eV, 2.697*eV, 2.757*eV, 2.820*eV,
      2.885*eV, 2.954*eV, 3.026*eV, 3.102*eV,
      3.181*eV, 3.265*eV, 3.353*eV, 3.446*eV,
      3.545*eV, 3.649*eV, 3.760*eV, 3.877*eV,
      4.002*eV, 4.136*eV, 6.260*eV };
  G4double RefractiveIndex[nEntries] =
    { 1.50, 1.50, 1.50, 1.50,
      1.50, 1.50, 1.50, 1.50,
      1.50, 1.50, 1.50, 1.50,
      1.50, 1.50, 1.50, 1.50,
      1.50, 1.50, 1.50, 1.50,
      1.50, 1.50, 1.50, 1.50,
      1.50, 1.50, 1.50, 1.50,
      1.50, 1.50, 1.50, 1.50,
      1.50, 1.50, 1.50 };
  G4MaterialPropertiesTable* myMPT = new G4MaterialPropertiesTable();
  myMPT->AddProperty("RINDEX", PhotonEnergy, RefractiveIndex, nEntries);
  //myMPT->AddProperty("ABSLENGTH", PhotonEnergy, Absorption, nEntries);
  mat->SetMaterialPropertiesTable(myMPT);

  return mat;
}

