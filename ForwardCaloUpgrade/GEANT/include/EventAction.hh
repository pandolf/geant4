//
// ********************************************************************
// ********************************************************************
//

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
//#include "HistoManager.hh"
//#include "DetectorConstruction.hh"

class RunAction;
class DetectorConstruction;
class HistoManager;

// Determination of EventAction-class

class EventAction : public G4UserEventAction
{
public:
  EventAction(RunAction*, DetectorConstruction*, HistoManager*);
  virtual ~EventAction();

  void  BeginOfEventAction(const G4Event*);
  void    EndOfEventAction(const G4Event*);
    
  void AddEcal(G4double de) { EnergyEcal += de; };
  void AddZero(G4double de) { EnergyZero += de; };
  void AddHcal(G4double de) { EnergyHcal += de; };       
  void AddAbs(G4double de)  { EnergyAbs  += de; };

  void AddHcalRange(G4double dl, G4int lay_hcal) {
                                 RangeHcal += dl;
                                 RangeHcalLay[lay_hcal] +=dl; };

  void AddEcalRange(G4double dl, G4int lay_ecal) {
                                 RangeEcal += dl;
                                 RangeEcalLay[lay_ecal] +=dl; };

  void fillEcalStep(G4double dEstep, G4int Lbin, G4int Rbin) {
                                     dEdL[Lbin] += dEstep; 
                                     dEdR[Rbin] += dEstep; };

  void fillHcalStep(G4double dEstep, G4int Lbin, G4int Rbin) {
                                     dEdLHcal[Lbin] += dEstep;
                                     dEdRHcal[Rbin] += dEstep; };

  void fillAbsStep(G4double dEstep, G4int Lbin, G4int Rbin) {
                                    dEdLAbs[Lbin] += dEstep;
                                    dEdRAbs[Rbin] += dEstep; };

  void fillEcalCell(G4int Lcell, G4double dEstep) {dECellsEcal[Lcell] += dEstep;};

  void fillEcalHits(G4int Lhits, G4double dEstep) {dEHitsEcal[Lhits] += dEstep;};

  G4int   GetEventNb()              {return evtNbOld;};

private:

   RunAction*  runAct;
   DetectorConstruction* detCon;
   HistoManager* myana;

   G4double    EnergyEcal, EnergyHcal, EnergyZero, EnergyAbs;
   G4double    RangeHcal,  RangeEcal;
   G4int       nRtot, nLtot, nRtoth, nLayers;
   G4int       nRtotAbs, nLtotAbs;
   G4int       nEcalCells;
   G4double    *dEdL, *dEdR, *dEdRHcal, *RangeEcalLay;
   G4double    *dEdLAbs, *dEdRAbs;
   G4double    *dECellsEcal, *dEHitsEcal;
   G4double    dEdLHcal[20], RangeHcalLay[20];
   G4int       printModulo;                     
   G4int       evtNbOld;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
