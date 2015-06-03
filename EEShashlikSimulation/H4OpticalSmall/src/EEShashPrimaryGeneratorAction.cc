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
 
  G4int myseed = time( NULL );
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
  G4double xBeam = (G4UniformRand()-0.5) * (15.) *mm;
  G4double yBeam = (G4UniformRand()-0.5) * (15.) *mm;

  //  G4double xBeam = x*mm;
  // G4double yBeam = y*mm;
  //    G4double xBeam = 0.;  // 0,0 with no smearing (well, there's still air scattering)
  //    G4double yBeam = 0.;
  // G4double xBeam = 12.-0.696; // centered on a fibre
  //  G4double yBeam = 12.-0.696;
  //   G4double xBeam = -12.+0.696; // centered on a fibre
  //   G4double yBeam = -12.+0.696; // centered on a fibre
  G4cout<<"xBeam:"<<xBeam<<" yBeam:"<<yBeam<<G4endl;
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

  //for cosmics
//  G4double xPos=(G4UniformRand()-0.5) * (15.) *mm;
//  G4double yPos=10 *cm;
//  G4double zPos=(G4UniformRand()) * (22.) *cm;
//  //  fParticleGun->SetParticlePosition(G4ThreeVector(0*mm,50*cm, 7*12*mm)); //starting to simulate one muon from cosmics
//  //    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,-1,0));
//  fParticleGun->SetParticlePosition(G4ThreeVector(xPos,yPos,zPos)); //starting to simulate one muon from cosmics
//  fParticleGun->SetParticleEnergy(cosmicRayMuonMomentum()*MeV);
//  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,-cosmicRayMuonAngle(),0));




  fParticleGun->GeneratePrimaryVertex(anEvent);

  xBeamPos = xBeam;
  yBeamPos = yBeam;
}






//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

double EEShashPrimaryGeneratorAction::cosmicRayMuonMomentum()
{
  // Average momentum is 3.0*GeV/c, cutoff is 120*GeV/c. Return value is in geant4 units (MeV)

  // Data from Kremer et al, Phys. Rev. Lett., Vol 83 no 21, p4241 (1999).
  // values are lower bin edge, bin average, mu+ rate, mu- rate
  // (laid out this silly way so verification with the paper is easy)
  // NOTE: units are GeV/c, and counts/(GeV/c m^2 sr s)
  static double vals[] = {
    0.0,0.0,0.0,0.0,
    0.2,0.25,14.0,11.0,
    0.3,0.35,16.8,13.6,
    0.4,0.47,17.2,14.4,
    0.55,0.62,16.6,13.5,
    0.70,0.78,15.6,13.3,
    0.85,0.92,14.8,12.1,
    1.0,1.1,13.0,11.0,
    1.2,1.3,12.0,10.1,
    1.4,1.5,10.2,8.7,
    1.6,1.84,9.1,7.3,
    2.1,2.49,6.6,5.2,
    2.94,3.49,4.12,3.38,
    4.12,4.78,2.53,1.98,
    5.5,6.21,1.61,1.25,
    7.0,8.37,0.90,0.69,
    10.0,12.42,0.389,0.309,
    15.5,18.85,0.138,0.108,
    23.0,26.68,0.063,0.046,
    31.1,36.69,0.028,0.019,
    43.6,51.47,0.0099,0.0071,
    61.1,72.08,0.0036,0.0030,
    85.6,100.96,0.0014,0.0012,
    120.0,120.0,0.0,0.0}; // cutoff at 120 GeV/c
  const int nvals = sizeof(vals)/sizeof(vals[0]);
  const int nbins = nvals/4 - 1;
  const int npdf=256;
  static double pdf[npdf];
  static double pmax = vals[4*nbins];
  static bool init=true;

  if(init) {
    // RandGeneral needs equal-sized bins for pdf[]
    // it returns a value in the range [0,1)
    //    hitsPerM2PerSecPerSterrad = 0.0;
    for(int i=0,ibin=0; i<npdf; ++i) {
      double p = (i+0.5)*pmax/npdf;
      while(p >= vals[4*ibin+5]) ++ibin;
      assert(ibin <= nbins);
      double f = (p - vals[4*ibin+1]) /
	(vals[4*ibin+5]-vals[4*ibin+1]);
      assert(0.0 <= f && f <= 1.0);
      pdf[i] = (1.0-f)*(vals[4*ibin+2]+vals[4*ibin+3]) +
	f*(vals[4*ibin+6]+vals[4*ibin+7]);
      //      hitsPerM2PerSecPerSterrad += pdf[i] * pmax/npdf;
    }
    init = false;
  }

  CLHEP::RandGeneral generator(pdf,npdf); // BUG in RandGeneral - cannot use new
  return generator.shoot() * pmax * GeV; //this is in MeV!

}

double EEShashPrimaryGeneratorAction::cosmicRayMuonAngle()
{
  //  returns a random angle distributed like the polar angle of cosmic ray muons. The return value is the polar angle from vertical (radians). It is cut off at 70 degrees. Note this is the distribution for muons of ~3 GeV/c, which is the average momentum. In fact, lower energy muons have a steeper distribution and higher ones have a flatter distribution. But this is a reasonable approximation. Particle Data Group, Review of Particle Properties, 2002. Section 23.3.1. 


  const int npdf=128;
  static double pdf[npdf];
  const double thetamax = 70.0*deg;
  static bool init = true;

  if(init) {
    // RandGeneral needs equal-sized bins for pdf[]
    // it returns a value in the range [0,1)
    //    sterradians = 0.0;
    G4double dtheta = thetamax / npdf;
    for(int i=0; i<npdf; ++i) {
      // Particle Data Group, Review of Particle Properties,
      // 2002. Section 23.3.1.
      double c = cos(dtheta*i);
      pdf[i] = c*c;
      //      sterradians += 2.0*pi*c*c*sin(dtheta*i)*dtheta;
    }
    init = false;
  }
  CLHEP::RandGeneral generator(pdf,npdf); // BUG in RandGeneral - cannot use new
  return generator.shoot() * thetamax;
}
