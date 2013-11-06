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

#include "DetectorPhysPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorPhysPrimaryGeneratorAction::DetectorPhysPrimaryGeneratorAction(DetectorPhysDetectorConstruction* DetectorPhysDC)
    :DetectorPhysDetector(DetectorPhysDC)
{
    G4int n_particle = 1;
    particleGun  = new G4ParticleGun(n_particle);
    
   // default particle kinematic
    
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName;
    G4ParticleDefinition* particle= particleTable->FindParticle(particleName="e-");
    particleGun->SetParticleDefinition(particle);
    particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,1.,0.));
    particleGun->SetParticleEnergy(1.*GeV);
    particleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,0.*cm));
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorPhysPrimaryGeneratorAction::~DetectorPhysPrimaryGeneratorAction()
{
  delete particleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorPhysPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

    particleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


