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
// $Id: EEShashEventAction.cc 75604 2013-11-04 13:17:26Z gcosmo $
// 
/// \file EEShashEventAction.cc
/// \brief Implementation of the EEShashEventAction class

#include "EEShashEventAction.hh"
#include "EEShashCalorimeterSD.hh"
#include "EEShashCalorHit.hh"
#include "EEShashAnalysis.hh"
#include "EEShashRunAction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashEventAction::EEShashEventAction(EEShashRunAction* runAct )
 : G4UserEventAction(),
   fAbsHCID(-1),
   fActHCID(-1),
   fRunAct(runAct)
{

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashEventAction::~EEShashEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashCalorHitsCollection* 
EEShashEventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  EEShashCalorHitsCollection* hitsCollection 
    = static_cast<EEShashCalorHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("EEShashEventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }         

  return hitsCollection;
}    

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashEventAction::PrintEventStatistics(
                              G4double absEdep, G4double absTrackLength,
                              G4double actEdep, G4double actTrackLength) const
{
  // print event statistics
  G4cout
     << "   Absorber: total energy: " 
     << std::setw(7) << G4BestUnit(absEdep, "Energy")
     << "       total track length: " 
     << std::setw(7) << G4BestUnit(absTrackLength, "Length")
     << G4endl
     << "        act: total energy: " 
     << std::setw(7) << G4BestUnit(actEdep, "Energy")
     << "       total track length: " 
     << std::setw(7) << G4BestUnit(actTrackLength, "Length")
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashEventAction::BeginOfEventAction(const G4Event* event)
{

 nRtot = 200;
 dRbin = 2.;
 dEdR     = new G4double[nRtot];

 for (G4int j=0; j<nRtot; ++j)    { dEdR[j] = 0.; }

 fRunAct->InitializePerEvent();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashEventAction::EndOfEventAction(const G4Event* event)
{  

  fRunAct->FillPerEvent();  

  // Get hits collections IDs (only once)
  if ( fAbsHCID == -1 ) {
    fAbsHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsHitsCollection");
    fActHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("ActHitsCollection");
  }

  // Get hits collections
  EEShashCalorHitsCollection* absHC = GetHitsCollection(fAbsHCID, event);
  EEShashCalorHitsCollection* actHC = GetHitsCollection(fActHCID, event);


  // Get hit with total values
  EEShashCalorHit* absHit = (*absHC)[absHC->entries()-1];
  EEShashCalorHit* actHit = (*actHC)[actHC->entries()-1];


  // Print per event (modulo n)
  //
  G4int eventID = event->GetEventID();
  G4int printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    G4cout << "---> End of event: " << eventID << G4endl;     

    PrintEventStatistics(
      absHit->GetEdep(), absHit->GetTrackLength(),
      actHit->GetEdep(), actHit->GetTrackLength());
  }  
  
  // Fill histograms, ntuple
  //



  // get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();


 
  // fill histograms
  analysisManager->FillH1(1, absHit->GetEdep());
  analysisManager->FillH1(2, actHit->GetEdep());
  analysisManager->FillH1(3, absHit->GetTrackLength());
  analysisManager->FillH1(4, actHit->GetTrackLength());



  // moliere radius:
  
  float EdepEcalRad = 0.;
  float rMoliere = 0.;
  for(G4int itr=0; itr<nRtot; itr++) {
    EdepEcalRad += dEdR[itr];
  }
  if( EdepEcalRad > 0. ) {
    G4double EdepTemp = 0.0;
    G4double xMolier = 0.0;
    for(G4int jtr=0; jtr<nRtot; jtr++) {
       G4double xrbin = dRbin*jtr + 0.5*dRbin;
       EdepTemp += dEdR[jtr];
       if( EdepTemp/EdepEcalRad < 0.90) rMoliere = xrbin;
    }
    analysisManager->FillH1(5, rMoliere);

  }

  
  // fill ntuple
  analysisManager->FillNtupleDColumn(0, absHit->GetEdep());
  analysisManager->FillNtupleDColumn(1, actHit->GetEdep());

  analysisManager->FillNtupleDColumn(2,actHit->GetEdep()/(actHit->GetEdep() +absHit->GetEdep()) );

  //LAST  analysisManager->FillNtupleDColumn(5, hodoHit->GetEdep());

  int nLayers = actHC->entries()-1; // the last hit is the total energy
  analysisManager->FillNtupleIColumn(3, nLayers);
  for( int i=0; i<nLayers; ++i ) {
    EEShashCalorHit* actHit_i = (*actHC)[i];
    analysisManager->FillNtupleDColumn(4+i, actHit_i->GetEdep());
  }


  analysisManager->AddNtupleRow();  


  delete dEdR;
  
}  


void EEShashEventAction::fillEcalStep(G4double dEstep, G4int Lbin, G4int Rbin) {

  dEdR[Rbin] += dEstep; 

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
