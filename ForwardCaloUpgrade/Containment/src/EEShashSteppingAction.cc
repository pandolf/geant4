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
/// \file electromagnetic/TestEm2/src/EEShashSteppingAction.cc
/// \brief Implementation of the EEShashSteppingAction class
//
// $Id: EEShashSteppingAction.cc 76259 2013-11-08 11:37:28Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "EEShashSteppingAction.hh"
#include "EEShashDetectorConstruction.hh"
#include "EEShashRunAction.hh"
#include "EEShashEventAction.hh"

#include "G4RunManager.hh"
#include "G4SteppingManager.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashSteppingAction::EEShashSteppingAction(EEShashDetectorConstruction* det, EEShashEventAction* evAct)
  :G4UserSteppingAction(),fDetector(det), fEvAct(evAct)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashSteppingAction::~EEShashSteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashSteppingAction::UserSteppingAction(const G4Step* step)
{ 
  
//// energy deposit
////
//G4double dEStep = step->GetTotalEnergyDeposit();

//
//if (dEStep > 0.) {
//  G4ThreeVector prePoint  = step->GetPreStepPoint()->GetPosition();
//  G4ThreeVector delta = step->GetPostStepPoint()->GetPosition() - prePoint;
//  prePoint += G4UniformRand()*delta;
//  G4double x = prePoint.x(), y = prePoint.y(), z = prePoint.z();
//  G4double radius = std::sqrt(x*x + y*y);
//  //   G4double offset = 0.5*fDetector->GetfullLength();
//  G4double offset = 0.;
//   G4int SlideNb = 1;
//  //   G4int SlideNb = G4int((z + offset)/fDetector->GetdLlength());
//  G4int RingNb  = 1;        
//  //    G4int RingNb  = G4int(radius/fDetector->GetdRlength());        
// 
//  fRunAct->FillPerStep(dEStep,SlideNb,RingNb);
//  }


   G4TouchableHandle touch = step->GetPreStepPoint()->GetTouchableHandle();


   G4VPhysicalVolume* volume = touch->GetVolume();


   G4double edep = step->GetTotalEnergyDeposit();
   G4ThreeVector prePoint  = step->GetPreStepPoint()->GetPosition();
   G4ThreeVector postPoint = step->GetPostStepPoint()->GetPosition();
   G4double stepl  = step->GetStepLength();
   G4double charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
   G4double totalP = step->GetTrack()->GetDynamicParticle()->GetTotalMomentum();
   G4double totalE = step->GetTrack()->GetTotalEnergy();
   G4Material* mat = step->GetTrack()->GetMaterial();
   G4MaterialPropertiesTable* propertiesTable = mat->GetMaterialPropertiesTable();

 
   if( volume == fDetector->GetEcal()) 
   {
     G4int nEcalLayer = touch->GetCopyNumber(1);
     if( edep > 0. ) {
       G4ThreeVector aPoint  = prePoint + G4UniformRand()*(postPoint - prePoint);
       G4double radius = std::sqrt(aPoint.x()*aPoint.x()+aPoint.y()*aPoint.y());
       G4double offset = 0.;

       G4int SlideNb = int( (aPoint.z() - offset) / 2. );
       if( SlideNb > 200 ) SlideNb = 200;
       if( fDetector->GetNbOfEcalLayers() !=1) SlideNb = nEcalLayer; 

       G4int RingNb  = int( radius / 0.2 );
       //       G4int RingNb  = int( radius / 2. );
       if( RingNb > 200 ) RingNb = 200;
       //       if( RingNb > 200 ) RingNb = 200;

       G4double response = edep;
       //G4double* birks = detector->GetEcalBirksConstant();
       //G4double* birkL3 = detector->GetEcalBirkL3Constant();

       //if(birks[0]*edep*stepl*charge != 0.) {
       //  G4double density = mat->GetDensity() / (g/cm3);
       //  G4double rkb     = birks[0]/density;
       //  if( int(birkL3[0]) > 0 )
       //    response = edep*getBirkL3(step, rkb, birkL3[1], birkL3[2]);
       //  else
       //    response = edep*getAttenuation(step, rkb, birks[1], birks[2]);
       //}

       fEvAct->fillEcalStep(response,SlideNb,RingNb);
      //eventaction->AddEcal(response);

     }

   }

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
