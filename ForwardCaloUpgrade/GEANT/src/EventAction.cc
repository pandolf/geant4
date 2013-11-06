//
// ********************************************************************
// ********************************************************************
//

#include "EventAction.hh"
#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "HistoManager.hh"

#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4VVisManager.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

// Constructor of EventAction-class and
// assignment of pinter runAct(run) <=> runAct=run

EventAction::EventAction(RunAction* run,DetectorConstruction* det,HistoManager* hist)
:runAct(run),detCon(det),myana(hist),printModulo(20)
{}

EventAction::~EventAction()
{}

// Member function which run at the start of each event

void EventAction::BeginOfEventAction(const G4Event* evt)
{  

// get bin number for histograms
//------------------------------

  nLayers  = detCon->GetNbOfEcalLayers();
  nRtot    = myana->GetnRtot();
  nLtot    = myana->GetnLtot();
  if( nLayers != 1 ) nLtot = nLayers;
  nRtoth   = myana->GetHcalnRtot();
  nRtotAbs = myana->GetAbsnRtot();
  nLtotAbs = myana->GetAbsnLtot();
  if( nLayers != 1 ) nLtotAbs = nLayers;
  nEcalCells   = detCon->GetNbOfEcalCells();

// initialize dynamic bin arrays
//-----------------------------
  dEdL     = new G4double[nLtot];
  dEdR     = new G4double[nRtot];
  dEdLAbs  = new G4double[nLtotAbs];
  dEdRAbs  = new G4double[nRtotAbs];
  dEdRHcal = new G4double[nRtoth];
  RangeEcalLay = new G4double[nLayers];
  dECellsEcal  = new G4double[nEcalCells];
  dEHitsEcal  = new G4double[nRtot*nRtot];

  G4int evtNb = evt->GetEventID();
  evtNbOld    = evt->GetEventID();

  if (evtNb%printModulo == 0) { 
    G4cout << "\n---> Begin of event: " << evtNb << G4endl;
//    CLHEP::HepRandom::showEngineStatus();
  }
 
// initialisation per event

  EnergyEcal = EnergyHcal = EnergyZero = EnergyAbs = 0. ;
  RangeHcal = RangeEcal = 0. ;
  for (G4int nl=0; nl<20; ++nl) { 
     dEdLHcal [nl] = 0.; 
     RangeHcalLay[nl] = 0.; 
  }
  for (G4int i=0; i<nLtot; ++i)    { dEdL[i] = 0.; }
  for (G4int j=0; j<nRtot; ++j)    { dEdR[j] = 0.; }     
  for (G4int k=0; k<nRtoth; ++k)   { dEdRHcal[k] = 0.; }     
  for (G4int l=0; l<nLayers; ++l)  { RangeEcalLay[l] = 0.; }     
  for (G4int m=0; m<nLtotAbs; ++m) { dEdLAbs[m] = 0.; }
  for (G4int n=0; n<nRtotAbs; ++n) { dEdRAbs[n] = 0.; }     
  for (G4int i=0; i<nEcalCells; ++i) { dECellsEcal[i] = 0.; }
  for (G4int j=0; j<nRtot*nRtot; ++j) { dEHitsEcal[j] = 0.; }
}

// Member function at the end of each event

void EventAction::EndOfEventAction(const G4Event* evt)
{

//accumulates statistics
//----------------------
  runAct->fillPerEvent(EnergyEcal, EnergyHcal, EnergyZero);
  
// fill histos
//------------
  myana-> FillEnergy(EnergyEcal, EnergyHcal, EnergyZero, EnergyAbs);

// fill Ecal and Hcal range of charged particles
//----------------------------------------------
  myana-> FillRange(RangeHcal, RangeEcal);

// fill Hcal longitudinal shower profile
//----------------------------------------
  myana-> FillHcalLongShape(dEdLHcal);  

// fill Hcal transverse shower profile
//-------------------------------------
  myana-> FillHcalTransShape(dEdRHcal);

// fill Ecal transverse shower profile
//-------------------------------------
  myana-> FillTransShape(dEdR);

// fill longitudinal shower profile
//----------------------------------
  myana-> FillLongShape(dEdL);

// fill Ecal absorber transverse shower profile
//----------------------------------------------
  myana-> FillAbsTransShape(dEdRAbs);
  
// fill Ecal absorber longitudinal shower profile
//------------------------------------------------
  myana-> FillAbsLongShape(dEdLAbs);

// fill Ecal cells energy
//------------------------
  myana-> FillCells(nEcalCells,dECellsEcal);   

// fill Ecal transverse hits energy
//---------------------------------
  myana-> FillEcalTransHits(dEHitsEcal);

//print per event (modulo n)
//---------------------------
  G4int evtNb = evt->GetEventID();

  if (evtNb%printModulo == 0) {
    G4cout << "---> End of event: " << evtNb << G4endl;	

    G4cout
       << " Total deposited energy: " 
       << "  ECAL = " << std::setw(7)
                     << G4BestUnit(EnergyEcal,"Energy")
       << ", HCAL = " << std::setw(7)
                     << G4BestUnit(EnergyHcal,"Energy")
       << ", ZERO = " << std::setw(7)
                     << G4BestUnit(EnergyZero,"Energy")
       << G4endl;
	  
  }

// delete dynamic bin arrays
//-------------------------
  delete dEdL;
  delete dEdR;
  delete dEdLAbs;
  delete dEdRAbs;
  delete dEdRHcal;
  delete RangeEcalLay;  
  delete dECellsEcal;
  delete dEHitsEcal;

}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
