//
// ********************************************************************
// ********************************************************************
//

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include <map>
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"

//class G4ParticleGun;
class G4Event;
class DetectorConstruction;
class PrimaryGeneratorActionMessenger;
//

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction(DetectorConstruction*);    
  virtual ~PrimaryGeneratorAction();

  void GeneratePrimaries(G4Event*);
  G4ParticleGun* GetParticleGun() {return (G4ParticleGun*)particleGun;};

  void SetVxSmearing(G4ThreeVector);
  void SetVxPosition(G4ThreeVector);

  void SetGenerator(G4VPrimaryGenerator* gen);
  void SetGenerator(G4String genname);

  G4VPrimaryGenerator* GetGenerator() const;
  G4String GetGeneratorName() const;
  
private:
  PrimaryGeneratorActionMessenger* actMessenger;
  G4VPrimaryGenerator*          particleGun;	  //pointer a to G4  class
  G4VPrimaryGenerator*          hepmcAscii;
  G4VPrimaryGenerator*          currentGenerator;
  DetectorConstruction*         Detector;         //pointer to the geometry

  G4int                         smearFlag, smearErea;
  G4double                      xVertex, yVertex, zVertex;
  G4double                      dRwidth;
  G4bool                        vertexDefined;
  
  G4String                      currentGeneratorName;
  std::map<G4String, G4VPrimaryGenerator*> gentypeMap;
    
};

// ====================================================================
// inline functions
// ====================================================================

inline void PrimaryGeneratorAction::SetGenerator
(G4VPrimaryGenerator* gen) 
{ 
  currentGenerator= gen; 
}

inline void PrimaryGeneratorAction::SetGenerator(G4String genname) 
{
  std::map<G4String, G4VPrimaryGenerator*>::iterator pos= gentypeMap.find(genname);
  if(pos != gentypeMap.end()) {
    currentGenerator= pos->second;
    currentGeneratorName= genname;
  }  
}

inline G4VPrimaryGenerator* PrimaryGeneratorAction::GetGenerator() const 
{ 
  return currentGenerator; 
}

inline G4String PrimaryGeneratorAction::GetGeneratorName() const 
{ 
  return currentGeneratorName; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


