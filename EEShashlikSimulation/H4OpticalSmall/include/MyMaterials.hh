#ifndef MyMaterials_hh
#define MyMaterials_hh

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"

class MyMaterials
{
private:

public:
  MyMaterials();
  ~MyMaterials();
  
  static G4Material* Air();
  
  static G4Material* Tungsten();

  static G4Material* OpticalGrease();
  
  static G4double fromNmToEv(G4double wavelength);
  static G4double fromEvToNm(G4double energy);
  static G4double CalculateSellmeier(int size, G4double indexZero, G4double *nVec, G4double *lVec, G4double wavelength);

};

#endif
