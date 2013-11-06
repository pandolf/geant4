//
// ====================================================================

#include "HistoManager.hh"
#include "HistoMessenger.hh"
#include "RunAction.hh"
#include "Randomize.hh"
#include "G4Poisson.hh"

// ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include <stdio.h>

// ====================================================================

HistoManager::HistoManager()
{
 for(G4int ih=0; ih<nhist; ih++) histo[ih]=0;
 hits = 0; 

 tree_tot  = 0;
 tree_vec  = 0;
 tree_ran  = 0;
 tree_cell = 0;

 fileName = "test_01.root";
 
 nLtot = nRtot = nLtotAbs = nRtotAbs = nRtotHcal = 200;
 dLbin = dRbin = dLbinAbs = dRbinAbs = dRbinHcal = 2.0;  

 LightYield    = 5.0E5;
 LightCollEff  = 0.03;
 LightCollUnif = 0.001;
 CellNoise     = 140.0*MeV; 

 RunNumber = 0;

 //gROOT->Reset();                         // ROOT style

 histoMessenger = new HistoMessenger(this);
}

HistoManager::~HistoManager()
{
  for(G4int ih=0; ih<nhist; ih++) delete histo[ih];
  delete hits;

  delete tree_tot;
  delete tree_vec;
  delete tree_ran;
  delete tree_cell;
  delete histoMessenger;
}

// histos definition
//-------------------
void HistoManager::Book(G4double Ekin, G4int nLayers)
{
// Turn on histo errors
//----------------------
  TH1::SetDefaultSumw2(true);

  histo[0] = new TH1D("Ecal", "Deposited Energy", 100, 0., 1.10*Ekin/GeV);
  histo[0]-> GetXaxis()-> SetTitle("E [GeV]");
  histo[0]-> SetFillColor(kBlue);
  histo[0]-> SetStats(1);

  histo[1] = new TH1D("Hcal", "Deposited Energy", 100, 0., 20.*Ekin/GeV); 
  histo[1]-> GetXaxis()-> SetTitle("E [MeV]");
  histo[1]-> SetFillColor(kBlue);
  histo[1]-> SetStats(1);

  histo[2] = new TH1D("Zero", "Deposited Energy", 100, 0., 10.*Ekin/GeV); 
  histo[2]-> GetXaxis()-> SetTitle("E [MeV]");
  histo[2]-> SetFillColor(kBlue);
  histo[2]-> SetStats(1);

  tree_tot = new TTree("Total", "Energy Calorimeter info");
  tree_tot-> Branch("ecal",&e_ecal,"e_ecal/D");
  tree_tot-> Branch("hcal",&e_hcal,"e_hcal/D");
  tree_tot-> Branch("zero",&e_zero,"e_zero/D");
  tree_tot-> Branch("abse",&e_abs ,"e_abs/D");

  tree_ran = new TTree("Range", "Range Calorimeter info");
  tree_ran-> Branch("hran",&r_hcal,"r_hcal/D");
  tree_ran-> Branch("eran",&r_ecal,"r_ecal/D");

  tree_vec = new TTree("Vector", "Layer energy info");
  tree_vec-> Branch("e_vec",e_vec,"e_vec[17]/D");

  tree_cell = new TTree("Cell", "Cell energy info");
  tree_cell-> Branch("n_cells",&n_cells,"n_cells/I");
  tree_cell-> Branch("e_dep",  e_dep,  "e_dep[n_cells]/D");
  tree_cell-> Branch("e_phot", e_phot,"e_phot[n_cells]/D");
  tree_cell-> Branch("e_unif", e_unif,"e_unif[n_cells]/D");
  tree_cell-> Branch("e_eff" , e_eff , "e_eff[n_cells]/D");

// Ecal transverse shower profile  in [mm]
//-----------------------------------------
    histo[3] = new TH1D("EcalTrShape","Ecal Transverse Shape",nRtot,0.,nRtot*dRbin);
    histo[3]-> GetXaxis()-> SetTitle("rho / mm");
    histo[3]-> GetYaxis()-> SetTitle("1/E dE/dbin");   
    histo[3]-> SetFillColor(kBlue);
    histo[3]-> SetStats(1);

// Ecal longitudinal shower profile in [mm] or per layer's number 
//----------------------------------------------------------------
    if( nLayers != 1) { nLtot = nLayers; dLbin = 1; }
    histo[4] = new TH1D("EcalLoShape","Ecal Longitudinal Shape",
                                               nLtot, 0., nLtot*dLbin);
    if( nLayers ==1) { histo[4]-> GetXaxis()-> SetTitle("z / mm");    }
    else             { histo[4]-> GetXaxis()-> SetTitle("z / layer"); }
    histo[4]-> GetYaxis()-> SetTitle("1/E dE/dbin");   
    histo[4]-> SetFillColor(kBlue);
    histo[4]-> SetStats(1);

// Ecal Molier radius [mm] in sensitive material (90% energy)
//-----------------------------------------------------------
    G4int nTemp = int(50./dRbin);
    histo[5] = new TH1D("EcalSensMol","Ecal Sens Molier",nTemp,0.,nTemp*dRbin);
    histo[5]-> GetXaxis()-> SetTitle("Molier radius / mm");
    histo[5]-> SetFillColor(kBlue);
    histo[5]-> SetStats(1);

// Hcal transverse shower profile  in [mm]
//-----------------------------------------
    histo[6] = new TH1D("HcalTrShape","Hcal Transverse Shape",
                                       nRtotHcal, 0., nRtotHcal*dRbinHcal);
    histo[6]-> GetXaxis()-> SetTitle("rho / mm");
    histo[6]-> GetYaxis()-> SetTitle("1/E dE/dbin");
    histo[6]-> SetFillColor(kBlue);
    histo[6]-> SetStats(1);

// Hcal longitudinal shower profile per layer's number
//------------------------------------------------------
    histo[7] = new TH1D("HcalLoShape","Hcal Longitudinal Shape", 18, 0., 18.);
    histo[7]-> GetXaxis()-> SetTitle("z / layer"); 
    histo[7]-> GetYaxis()-> SetTitle("1/E dE/dbin");
    histo[7]-> SetFillColor(kBlue);
    histo[7]-> SetStats(1);

// Ecal transverse shower profile [mm] in absorber
//-------------------------------------------------   
    histo[8] = new TH1D("AbsTrShape","Ecal Abs Transverse Shape",
                                   nRtotAbs, 0., nRtotAbs*dRbinAbs);
    histo[8]-> GetXaxis()-> SetTitle("rho / mm");
    histo[8]-> GetYaxis()-> SetTitle("1/E dE/dbin");
    histo[8]-> SetFillColor(kBlue);
    histo[8]-> SetStats(1);

// Ecal longitudinal shower profile in [mm] or per layer's number
//----------------------------------------------------------------
    if( nLayers != 1) { nLtotAbs = nLayers; dLbinAbs = 1; }
    histo[9] = new TH1D("AbsLoShape","Ecal Abs Longitudinal Shape",
                                      nLtotAbs, 0., nLtotAbs*dLbinAbs);
    if( nLayers ==1) { histo[9]-> GetXaxis()-> SetTitle("z / mm");    }
    else             { histo[9]-> GetXaxis()-> SetTitle("z / layer"); }
    histo[9]-> GetYaxis()-> SetTitle("1/E dE/dbin");
    histo[9]-> SetFillColor(kBlue);
    histo[9]-> SetStats(1);

// Ecal Molier radius [mm] in absorber material (90% energy)
//-----------------------------------------------------------
    G4int naTemp = int(50./dRbinAbs);
    histo[10] = new TH1D("EcalAbsMol","Ecal Abs Molier",naTemp,0.,naTemp*dRbinAbs);
    histo[10]-> GetXaxis()-> SetTitle("Molier radius / mm");
    histo[10]-> SetFillColor(kBlue);
    histo[10]-> SetStats(1);

// Ecal sensitive transverse hit points distributions (Y vs Z) 
//-------------------------------------------------------------
    
    hits = new TH2D("EcalHitPoint","Ecal Hit points",nRtot,-0.5*nRtot*dRbin,
                    0.5*nRtot*dRbin, nRtot,-0.5*nRtot*dRbin,0.5*nRtot*dRbin);
    hits-> GetXaxis()-> SetTitle("X - axis / mm");
    hits-> GetYaxis()-> SetTitle("Y - axis / mm");
    hits-> SetFillColor(kBlue);
    hits-> SetStats(1);

  return;
}

//==> reset histos ===============================

  void HistoManager::Clear()
{
  for(G4int ih=0;ih<nhist;ih++) histo[ih]=0;
  hits = 0;

  tree_tot = 0;
  tree_vec = 0;
  tree_ran = 0;
  tree_cell= 0;

  return;
}

//==> write out histos in the output file ===============

//  void HistoManager::Save(G4String fileName)
  void HistoManager::Save()
{

  TFile* file = new TFile(fileName, "RECREATE", "Geant4 ROOT analysis");

  for(G4int ih=0; ih<nhist; ih++) histo[ih]->Write();
  hits->Write();

  tree_tot->  Write();
  tree_vec->  Write();
  tree_ran->  Write();
  tree_cell-> Write();

  file-> Close();
  delete file;

  return;
}

//==> Fill histo with deposited energy ====================

   void HistoManager::FillEnergy(G4double ecal, G4double hcal, G4double zero,
                                                               G4double absor)
   {

     histo[0]-> Fill(ecal/GeV);
     histo[1]-> Fill(hcal/MeV);
     histo[2]-> Fill(zero/MeV);   

     e_ecal = ecal;
     e_hcal = hcal;
     e_zero = zero;
     e_abs  = absor; 
     tree_tot-> Fill();
   }

// Fill Ecal and Hcal range of charged particles
//----------------------------------------------
   void HistoManager::FillRange(G4double rhcal, G4double recal)
   {
     r_hcal = rhcal;
     r_ecal = recal;
     tree_ran-> Fill();
   }  

// Fill Ecal cells
//----------------
   void HistoManager::FillCells(G4int ncells, G4double* p_cell)
   {
     n_cells = ncells;
     for( G4int ic=0; ic<n_cells; ic++) { 
       G4double EffLight = 0.001*LightYield*LightCollEff;  // photos/MeV
       G4double MeanNbPhotons = EffLight * p_cell[ic];
       G4int NbPhotons = G4Poisson(MeanNbPhotons);

       G4double z0 = G4RandGauss::shoot(0.0,1.0); 
       G4double ecell_unif = 0.0;
       if( EffLight > 0.0) ecell_unif = (NbPhotons/EffLight)*(1.+z0*LightCollUnif);

       G4double sigma = G4RandGauss::shoot(0.0,CellNoise); 
       G4double ecell_eff = ecell_unif + sigma;

       e_dep[ic]  = p_cell[ic];
       e_phot[ic] = G4double(NbPhotons);
       e_unif[ic] = ecell_unif;
       e_eff[ic]  = ecell_eff;
     }
     tree_cell-> Fill();
   }

// Fill Ecal transverse shower profile in sensitive material
//-----------------------------------------------------------
   void HistoManager::FillTransShape(G4double* p_tra)
   {
     EdepEcalRad = 0.;
     for(G4int itr=0; itr<nRtot; itr++) EdepEcalRad += p_tra[itr];
     if( EdepEcalRad > 0. ) { 
       G4double EdepTemp = 0.0;
       G4double xMolier = 0.0;  
       for(G4int jtr=0; jtr<nRtot; jtr++) {
          G4double xrbin = dRbin*jtr + 0.5*dRbin;
          histo[3]-> Fill(xrbin,p_tra[jtr]/EdepEcalRad);
          EdepTemp += p_tra[jtr];
          if( EdepTemp/EdepEcalRad < 0.90) xMolier = xrbin;
       }
       histo[5]-> Fill(xMolier);
     }
   }

// Fill Ecal longitudinal shower profile in sensitive material
//------------------------------------------------------------
   void HistoManager::FillLongShape(G4double* p_lon)
   {
     EdepEcalLong = 0.;
     for(G4int ilo=0; ilo<nLtot; ilo++) EdepEcalLong += p_lon[ilo];
     if( EdepEcalLong > 0. ) {
       for(G4int jlo=0; jlo<nLtot; jlo++) {
          G4double xlbin = dLbin*jlo + 0.5*dLbin; 
          histo[4]-> Fill(xlbin,p_lon[jlo]/EdepEcalLong);
       }
     }
   }

// Fill Hcal transverse shower profile in sensitive material
//-----------------------------------------------------------
   void HistoManager::FillHcalTransShape(G4double* ph_tra)
   {
     EdepHcalRad = 0.;
     for(G4int ihtr=0; ihtr<nRtotHcal; ihtr++) EdepHcalRad += ph_tra[ihtr];
     if( EdepHcalRad > 0. ) {
       for(G4int jhtr=0; jhtr<nRtotHcal; jhtr++) {
          G4double xhrbin = dRbinHcal*jhtr + 0.5*dRbinHcal;
          histo[6]-> Fill(xhrbin,ph_tra[jhtr]/EdepHcalRad);
       }
     }
   }

// Fill Hcal longitudinal shower profile in sensitive material
//--------------------------------------------------------------
   void HistoManager::FillHcalLongShape(G4double* ph_lon)
   {
     EdepHcalLong = 0.;
     for(G4int ihlo=0; ihlo<18; ihlo++) EdepHcalLong += ph_lon[ihlo];
     if( EdepHcalLong > 0. ) {
       for(G4int jhlo=0; jhlo<18; jhlo++) {
          G4double xhlbin = jhlo + 0.5;
          histo[7]-> Fill(xhlbin,ph_lon[jhlo]/EdepHcalLong);
       }
     }  
     for(G4int kfi=0; kfi<17; kfi++) e_vec[kfi] = ph_lon[kfi];
     tree_vec-> Fill();
   }    

// Fill Ecal transverse shower shape in absorber
//-----------------------------------------------
   void HistoManager::FillAbsTransShape(G4double* pa_tra)
   {
     EdepAbsRad = 0.;
     for(G4int itr=0; itr<nRtotAbs; itr++) EdepAbsRad += pa_tra[itr];
     if( EdepAbsRad > 0. ) {
       G4double EdepTemp = 0.0;
       G4double xMolier = 0.0;
       for(G4int jtr=0; jtr<nRtotAbs; jtr++) {
          G4double xrbin = dRbinAbs*jtr + 0.5*dRbinAbs;
          histo[8]-> Fill(xrbin,pa_tra[jtr]/EdepAbsRad);
          EdepTemp += pa_tra[jtr];
          if( EdepTemp/EdepAbsRad < 0.90 ) xMolier = xrbin;
       }
       histo[10]->Fill(xMolier);
     }
   }

// Fill Ecal longitudinal shower profile in absorber
//---------------------------------------------------
   void HistoManager::FillAbsLongShape(G4double* pa_lon)
   {
     EdepAbsLong = 0.;
     for(G4int ilo=0; ilo<nLtotAbs; ilo++) EdepAbsLong += pa_lon[ilo];
     if( EdepAbsLong > 0. ) {
       for(G4int jlo=0; jlo<nLtotAbs; jlo++) {
          G4double xlbin = dLbinAbs*jlo + 0.5*dLbinAbs;
          histo[9]-> Fill(xlbin,pa_lon[jlo]/EdepAbsLong);
       }
     }
   }

// Fill Ecal hit point distribution in sensitive media
//---------------------------------------------------
   void HistoManager::FillEcalTransHits(G4double* pa_lon)
   {   
     EdepEcalHits = 0.;
     for(G4int ij=0; ij<nRtot*nRtot; ij++) EdepEcalHits += pa_lon[ij];
     if( EdepEcalHits > 0. ) {
       for(G4int ij=0; ij<nRtot*nRtot; ij++) {
           G4int iy_ind = ij / nRtot;
           G4int ix_ind = ij - iy_ind*nRtot; 
           G4double xlbin = -0.5*dRbin*nRtot + dRbin*ix_ind + 0.5*dRbin;
           G4double ylbin = -0.5*dRbin*nRtot + dRbin*iy_ind + 0.5*dRbin;
           if( pa_lon[ij] > 0.0)  hits->Fill(xlbin,ylbin,pa_lon[ij]/EdepEcalHits);
       }
     }
   }

// Set binning for Ecal transverse shower profile
//------------------------------------------------
  void HistoManager::SetSensRBining(G4ThreeVector Value)
  { 
    nRtot   = Value(0);
    dRbin   = Value(1)*mm;
  }

// Set binning for Ecal longitudinal shower profile
//--------------------------------------------------
  void HistoManager::SetSensLBining(G4ThreeVector Value)
  {
    nLtot   = Value(0);
    dLbin   = Value(1)*mm;
  }       

// Set binning for Hcal transverse shower profile
//------------------------------------------------
  void HistoManager::SetHcalRBining(G4ThreeVector Value)
  {
    nRtotHcal   = Value(0);
    dRbinHcal   = Value(1)*mm;
  }

// Set binning for Ecal Absorber transverse shower profile 
//---------------------------------------------------------
  void HistoManager::SetAbsRBining(G4ThreeVector Value)
  {
    nRtotAbs   = Value(0);
    dRbinAbs   = Value(1)*mm;
  }
   
// Set binning for Ecal Absorber longitudinal shower profile
//-----------------------------------------------------------
  void HistoManager::SetAbsLBining(G4ThreeVector Value)
  {
    nLtotAbs   = Value(0);
    dLbinAbs   = Value(1)*mm;
  }

// Set Ecal response parameter (light yield, light collection efficiency
// and light collection ununiformity) ----------------------------------
//------------------------------------
  void HistoManager::SetEcalResponse(G4ThreeVector Value)
  {
    LightYield      = Value(0);
    LightCollEff    = Value(1);
    LightCollUnif   = Value(2);
  }

// Set Ecal cell noise [MeV]
//--------------------------
  void HistoManager::SetEcalCellNoise(G4double Value)
  { 
    CellNoise = Value;
  }

// Set Job Run Number 
//--------------------

  void HistoManager::SetJobRunNumber(G4int Value)
  {
    RunNumber = Value;
  }

// Set ROOT file name
//--------------------
  void HistoManager::SetFileName(G4String userFile)
  {
    fileName = userFile;
  }

//===========================================================
