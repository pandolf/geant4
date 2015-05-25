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
// $Id: EEShashPrimaryGeneratorAction.cc 75215 2013-10-29 16:07:06Z gcosmo $
// 
/// \file EEShashPrimaryGeneratorAction.cc
/// \brief Implementation of the EEShashPrimaryGeneratorAction class

#include "common.h"
#include "EEShashPrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"


G4double xBeamPos;
G4double yBeamPos;
G4double fibre0;
G4double fibre1;
G4double fibre2;
G4double fibre3;
G4double EOpt_0;
G4double EOpt_1;
G4double EOpt_2;
G4double EOpt_3;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashPrimaryGeneratorAction::EEShashPrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(),
   fParticleGun(0)
{
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // default particle kinematic
  //
  G4ParticleDefinition* particleDefinition 
    = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(100. *MeV);

  rand_ = new TRandom3(13);

  G4Random::setTheEngine(new CLHEP::RanecuEngine);
 
  G4int myseed = 1234;
  G4Random::setTheSeed(myseed);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashPrimaryGeneratorAction::~EEShashPrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // This function is called at the begining of event

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume
  // from G4LogicalVolumeStore
  //
  G4double worldZHalfLength = 0;
  G4LogicalVolume* worlLV
    = G4LogicalVolumeStore::GetInstance()->GetVolume("World");
  G4Box* worldBox = 0;
  if ( worlLV) worldBox = dynamic_cast< G4Box*>(worlLV->GetSolid()); 
  if ( worldBox ) {
    worldZHalfLength = worldBox->GetZHalfLength();  
  }
  else  {
    G4ExceptionDescription msg;
    msg << "World volume of box not found." << G4endl;
    msg << "Perhaps you have changed geometry." << G4endl;
    msg << "The gun will be place in the center.";
    G4Exception("EEShashPrimaryGeneratorAction::GeneratePrimaries()",
      "MyCode0002", JustWarning, msg);
  } 


  // smear gun 
  // float x = rand_->Gaus( 0., 0.3 );
  // float y = rand_->Gaus( 0., 0.3 );
  //float x = rand_->Uniform( 0., 30.0 );
  //float y = rand_->Uniform( 0., 30.0 );
  G4double xBeam = (G4UniformRand()-0.5) * (1.) *mm;
  G4double yBeam = (G4UniformRand()-0.5) * (1.) *mm;
  //  G4double xBeam = x*mm;
  //G4double yBeam = y*mm;
  //G4double xBeam = 0.;  // 0,0 with no smearing (well, there's still air scattering)
  //G4double yBeam = 0.;
  //G4double xBeam = 12.-0.696; // centered on a fibre
  //G4double yBeam = 12.-0.696;
  fibre0=0;
  fibre1=0;
  fibre2=0;
  fibre3=0;
  EOpt_0=0;
  EOpt_1=0;
  EOpt_2=0;
  EOpt_3=0;
  
  // Set gun position
  fParticleGun->SetParticlePosition(G4ThreeVector(xBeam, yBeam, -1.587*m));

  fParticleGun->GeneratePrimaryVertex(anEvent);

  xBeamPos = xBeam;
  yBeamPos = yBeam;
}











//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

