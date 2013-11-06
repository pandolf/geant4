//
// ********************************************************************
// ********************************************************************
//

#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "HepMCG4AsciiReader.hh"
#include "Randomize.hh"

#include <stdio.h>
#define twopi 6.2831853


PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* det)
:Detector(det),smearFlag(0),smearErea(0),xVertex(-110.),yVertex(0.),
 zVertex(0.),dRwidth(5.0),vertexDefined(false)
{
  G4int n_particle = 1;
  
// default generator is particle gun.
//===================================
  currentGenerator= particleGun= new G4ParticleGun(n_particle);
  currentGeneratorName= "particleGun";
  hepmcAscii= new HepMCG4AsciiReader();
  gentypeMap["particleGun"]= particleGun;
  gentypeMap["hepmcAscii"]= hepmcAscii;
  
// default particle kinematic
//============================

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;

  G4ParticleDefinition* particle
                    = particleTable->FindParticle(particleName="pi-");
  ((G4ParticleGun*)particleGun)->SetParticleDefinition(particle);

// set particle energy and position
//==================================

  ((G4ParticleGun*)particleGun)->SetParticleMomentumDirection(G4ThreeVector(1.0,0.0,0.0));
  ((G4ParticleGun*)particleGun)->SetParticleEnergy(10.*GeV);
  ((G4ParticleGun*)particleGun)->SetParticlePosition( G4ThreeVector(xVertex*cm,yVertex*cm,zVertex*cm) );
//  particleGun->SetParticlePosition(G4ThreeVector(-10.0*cm,0.0*cm,0.0*cm));

  actMessenger = new PrimaryGeneratorActionMessenger(this);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete currentGenerator;
  delete actMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  G4double x0, y0, z0;
  if(vertexDefined) {
    x0 = xVertex;
    y0 = yVertex;
    z0 = zVertex;
  }
  else {
    x0 = -110.0*cm;
    y0 =    0.0*cm;
    z0 =    0.0*cm; 
  }

  if( smearFlag > 0 ) {
    if(smearErea == 0 ) {
      G4double r0   = dRwidth*sqrt(G4UniformRand());
      G4double phi0 = twopi*G4UniformRand();
      z0   = r0*cos(phi0);
      y0   = r0*sin(phi0);
    }
    else {
      y0 = dRwidth*(G4UniformRand()-0.5);
      z0 = dRwidth*(G4UniformRand()-0.5);
    }
  } 
  particleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
  
  if(currentGenerator)
    currentGenerator-> GeneratePrimaryVertex(anEvent);
  else 
    //   G4Exception("generator is not instantiated.");
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()", "PrimaryGeneratorAction", FatalException, "generator is not instantiated");

}

void PrimaryGeneratorAction::SetVxSmearing(G4ThreeVector Value)
{
  smearFlag = G4int( Value(0) );
  smearErea = G4int( Value(1) );
  dRwidth   = G4double( Value(2) )*mm;
}

void PrimaryGeneratorAction::SetVxPosition(G4ThreeVector Value)
{
  vertexDefined = true;
  xVertex = G4double( Value(0) )*cm;
  yVertex = G4double( Value(1) )*cm;
  zVertex = G4double( Value(2) )*cm;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

