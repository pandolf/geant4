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
// $Id: EEShashRunAction.cc 75215 2013-10-29 16:07:06Z gcosmo $
//
/// \file EEShashRunAction.cc
/// \brief Implementation of the EEShashRunAction class

#include "EEShashRunAction.hh"
#include "EEShashAnalysis.hh"
#include "EEShashDetectorConstruction.hh"
#include "EEShashEventAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashRunAction::EEShashRunAction( EEShashDetectorConstruction* det)
  : G4UserRunAction(), fDet(det),
  f_nLbin(100),f_nRbin(100)
{ 
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);     

  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in EEShashAnalysis.hh
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;


  analysisManager->SetVerboseLevel(1);
  analysisManager->SetFirstHistoId(1);


  G4double Ekin = 5*GeV;
  //  G4double Ekin = fKin->GetParticleGun()->GetParticleEnergy();
  G4int    nLbin = fDet->GetnLtot();
  G4int    nRbin = fDet->GetnRtot();
  G4double dLradl = fDet->GetdLradl();
  G4double dRradl = fDet->GetdRradl();

  // Book histograms, ntuple
  //
  
  // Creating histograms
  analysisManager->CreateH1("edep_abs","Edep in absorber", 100, 0., 800*MeV);
  analysisManager->CreateH1("edep_act","Edep in active", 100, 0., 100*MeV);
  analysisManager->CreateH1("trkl_abs","trackL in absorber", 100, 0., 1*m);
  analysisManager->CreateH1("trkl_act","trackL in active", 100, 0., 50*cm);


  analysisManager->CreateH1( "Rmoliere","Moliere Radius",50,0.,50.);
                                    
  analysisManager->CreateH1( "h6","rms on longit Edep (% of E inc)", nLbin,0.,nLbin*dLradl);
  G4double Zmin=0.5*dLradl, Zmax=Zmin+nLbin*dLradl;
  analysisManager->CreateH1( "h7","cumul longit energy dep (% of E inc)", nLbin,Zmin,Zmax);
                                    
  analysisManager->CreateH1( "h8","rms on cumul longit Edep (% of E inc)", nLbin,Zmin,Zmax);

  analysisManager->CreateH1( "h9","radial energy profile (% of E inc)", nRbin,0.,nRbin*dRradl);
                                                                        
  analysisManager->CreateH1( "h10","rms on radial Edep (% of E inc)", nRbin,0.,nRbin*dRradl);            
  G4double Rmin=0.5*dRradl, Rmax=Rmin+nRbin*dRradl;
  analysisManager->CreateH1("h11","cumul radial energy dep (% of E inc)", nRbin,Rmin,Rmax);

  analysisManager->CreateH1("h12","rms on cumul radial Edep (% of E inc)", nRbin,Rmin,Rmax);                     

  // Creating ntuple
  //
  analysisManager->CreateNtuple("EEShash", "Edep and TrackL");
  analysisManager->CreateNtupleDColumn("Eabs");
  analysisManager->CreateNtupleDColumn("Eact");
  analysisManager->CreateNtupleDColumn("Eact/(Eact+Eabs)");


  analysisManager->CreateNtupleIColumn("nLayers");
  // didnt find an easier way to do this
  for( unsigned i=0; i<90; ++i )
    analysisManager->CreateNtupleDColumn(Form("Eact_%d", i));


  analysisManager->FinishNtuple();

  Reset();
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashRunAction::Reset()
{    
  f_nLbin = fDet->GetnLtot();
  f_dEdL.resize(f_nLbin);
  fSumELongit.resize(f_nLbin);
  fSumELongitCumul.resize(f_nLbin);
  fSumE2Longit.resize(f_nLbin);
  fSumE2LongitCumul.resize(f_nLbin);

  f_nRbin = fDet->GetnRtot();
  f_dEdR.resize(f_nRbin);
  fSumERadial.resize(f_nRbin);
  fSumERadialCumul.resize(f_nRbin);
  fSumE2Radial.resize(f_nRbin);
  fSumE2RadialCumul.resize(f_nRbin);
  

  //initialize arrays of cumulative energy deposition
  //
  for (G4int i=0; i<f_nLbin; i++) 
     fSumELongit[i]=fSumE2Longit[i]=fSumELongitCumul[i]=fSumE2LongitCumul[i]=0.;
  
  for (G4int j=0; j<f_nRbin; j++)
     fSumERadial[j]=fSumE2Radial[j]=fSumERadialCumul[j]=fSumE2RadialCumul[j]=0.;

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashRunAction::~EEShashRunAction()
{
  delete G4AnalysisManager::Instance();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashRunAction::BeginOfRunAction(const G4Run* /*run*/)
{ 
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  
  // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  G4String fileName = "EEShash";
  analysisManager->OpenFile(fileName);

}




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashRunAction::InitializePerEvent()
{
  //initialize arrays of energy deposit per bin
  for (G4int i=0; i<f_nLbin; i++)
     { f_dEdL[i] = 0.; }
     
  for (G4int j=0; j<f_nRbin; j++)
     { f_dEdR[j] = 0.; }     

}





//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashRunAction::FillPerEvent()
{
  //accumulate statistic
  //
  G4double dLCumul = 0.;
  for (G4int i=0; i<f_nLbin; i++)
    {
      fSumELongit[i]  += f_dEdL[i];
      fSumE2Longit[i] += f_dEdL[i]*f_dEdL[i];
      dLCumul        += f_dEdL[i];
      fSumELongitCumul[i]  += dLCumul;
      fSumE2LongitCumul[i] += dLCumul*dLCumul;
    }

  G4double dRCumul = 0.;
  for (G4int j=0; j<f_nRbin; j++)
    {
      fSumERadial[j]  += f_dEdR[j];
      fSumE2Radial[j] += f_dEdR[j]*f_dEdR[j];
      dRCumul        += f_dEdR[j];
      fSumERadialCumul[j]  += dRCumul;
      fSumE2RadialCumul[j] += dRCumul*dRCumul;
    }

  /*
  fSumChargTrLength  += fChargTrLength;
  fSum2ChargTrLength += fChargTrLength*fChargTrLength;
  fSumNeutrTrLength  += fNeutrTrLength;
  fSum2NeutrTrLength += fNeutrTrLength*fNeutrTrLength;
  */

}













//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashRunAction::EndOfRunAction(const G4Run* run)
{
  // print histogram statistics
  //
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->GetH1(1) ) {
    G4cout << "\n ----> print histograms statistic ";
    if(isMaster) {
      G4cout << "for the entire run \n" << G4endl; 
    }
    else {
      G4cout << "for the local thread \n" << G4endl; 
    }
    
    G4cout << " EAbs : mean = " 
       << G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(1)->rms(),  "Energy") << G4endl;
    
    G4cout << " Eact : mean = " 
       << G4BestUnit(analysisManager->GetH1(2)->mean(), "Energy") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(2)->rms(),  "Energy") << G4endl;
    
    G4cout << " LAbs : mean = " 
      << G4BestUnit(analysisManager->GetH1(3)->mean(), "Length") 
      << " rms = " 
      << G4BestUnit(analysisManager->GetH1(3)->rms(),  "Length") << G4endl;

    G4cout << " Lact : mean = " 
      << G4BestUnit(analysisManager->GetH1(4)->mean(), "Length") 
      << " rms = " 
      << G4BestUnit(analysisManager->GetH1(4)->rms(),  "Length") << G4endl;
  }



 




  
  G4int NbOfEvents = run->GetNumberOfEvent();

  G4double kinEnergy = 5*GeV;
  //  G4double kinEnergy = fKin->GetParticleGun()->GetParticleEnergy();
  assert(NbOfEvents*kinEnergy > 0);


  G4double mass= 511*keV;
  //  G4double mass=fKin->GetParticleGun()->GetParticleDefinition()->GetPDGMass();
  G4double norme = 100./(NbOfEvents*(kinEnergy+mass));
 


  //longitudinal
  //
  G4double dLradl = fDet->GetdLradl();

  MyVector MeanELongit(f_nLbin),      rmsELongit(f_nLbin);
  MyVector MeanELongitCumul(f_nLbin), rmsELongitCumul(f_nLbin);

  //  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  G4int i;
  for (i=0; i<f_nLbin; i++) {
    MeanELongit[i] = norme*fSumELongit[i];
    rmsELongit[i] = 
      norme*std::sqrt(std::abs(NbOfEvents*fSumE2Longit[i]
                             - fSumELongit[i]*fSumELongit[i]));
      
    MeanELongitCumul[i] = norme*fSumELongitCumul[i];
    rmsELongitCumul[i] = norme*std::sqrt(std::abs(NbOfEvents*
      fSumE2LongitCumul[i] - fSumELongitCumul[i]*fSumELongitCumul[i]));
    G4double bin = (i+0.5)*dLradl;
    analysisManager->FillH1(5, bin,MeanELongit[i]/dLradl);
    analysisManager->FillH1(6, bin, rmsELongit[i]/dLradl);      
    bin = (i+1)*dLradl;
    analysisManager->FillH1(7, bin,MeanELongitCumul[i]);
    analysisManager->FillH1(8, bin, rmsELongitCumul[i]);
  }

  //radial
  //
  G4double dRradl = fDet->GetdRradl();

  MyVector MeanERadial(f_nRbin),      rmsERadial(f_nRbin);
  MyVector MeanERadialCumul(f_nRbin), rmsERadialCumul(f_nRbin);

  for (i=0; i<f_nRbin; i++) {
    MeanERadial[i] = norme*fSumERadial[i];
    rmsERadial[i] = norme*std::sqrt(std::abs(NbOfEvents*fSumE2Radial[i]
                                    - fSumERadial[i]*fSumERadial[i]));
      
    MeanERadialCumul[i] = norme*fSumERadialCumul[i];
    rmsERadialCumul[i] = 
      norme*std::sqrt(std::abs(NbOfEvents*fSumE2RadialCumul[i] 
                             - fSumERadialCumul[i]*fSumERadialCumul[i]));

    G4double bin = (i+0.5)*dRradl;
    analysisManager->FillH1(9, bin,MeanERadial[i]/dRradl);
    analysisManager->FillH1(10, bin, rmsERadial[i]/dRradl);      
    bin = (i+1)*dRradl;
    analysisManager->FillH1(11, bin,MeanERadialCumul[i]);
    analysisManager->FillH1(12, bin, rmsERadialCumul[i]);      
  }

  //find Moliere confinement
  //
  const G4double EMoliere = 90.;
  G4double iMoliere = 0.;
  if ((MeanERadialCumul[0]       <= EMoliere) &&
      (MeanERadialCumul[f_nRbin-1] >= EMoliere)) {
    G4int imin = 0;
    while( (imin < f_nRbin-1) && (MeanERadialCumul[imin] < EMoliere) ) 
      { ++imin; }
    G4double ratio = (EMoliere - MeanERadialCumul[imin]) /
                     (MeanERadialCumul[imin+1] - MeanERadialCumul[imin]);
    iMoliere = 1. + imin + ratio;
  }                       




    G4cout << G4endl << G4endl << G4endl;

    G4cout << "                  RADIAL PROFILE                   "
           << "      CUMULATIVE  RADIAL PROFILE" << G4endl << G4endl;

    G4cout << "        bin   " << "           Mean         rms         "
           << "        bin "   << "           Mean      rms \n" << G4endl;

    for (i=0; i<f_nRbin; i++) {
      G4double inf=i*dRradl, sup=inf+dRradl;

      G4cout << std::setw(8) << inf << "->"
             << std::setw(5) << sup << " radl: "
             << std::setw(7) << MeanERadial[i] << "%  "
             << std::setw(9) << rmsERadial[i] << "%       "
             << "      0->" << std::setw(5) << sup << " radl: "
             << std::setw(7) << MeanERadialCumul[i] << "%  "
             << std::setw(7) << rmsERadialCumul[i] << "% "
             <<G4endl;
    }



  if (iMoliere > 0. ) {
    G4double RMoliere1 = iMoliere*fDet->GetdRradl();
    G4double RMoliere2 = iMoliere*fDet->GetdRlength();          
    G4cout << "\n " << EMoliere << " % confinement: radius = "
           << RMoliere1 << " radl  ("
           << G4BestUnit( RMoliere2, "Length") << ")" << "\n" << G4endl;
  }    


  // save histograms & ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();

  //hitsFile_->cd();
  //hitsTree_->Write();
  //hitsFile_->Close();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
