//
// ====================================================================

#ifndef HistoManager_h
#define HistoManager_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"

// ====================================================================

class TH1D;
class TH2D;
class TTree;

class HistoMessenger;

 const G4int  nhist = 11; 

class HistoManager 
{

  public:

    HistoManager();
   ~HistoManager();

    void Book(G4double, G4int);
    void Clear();
    void Save();

    void FillEnergy(G4double, G4double, G4double, G4double);
    void FillRange(G4double, G4double);

    void FillHcalTransShape(G4double*);
    void FillHcalLongShape(G4double*);

    void FillTransShape(G4double*);
    void FillLongShape(G4double*);

    void FillAbsTransShape(G4double*);
    void FillAbsLongShape(G4double*);

    void FillCells(G4int, G4double*);
    void FillEcalTransHits(G4double*);

    void SetFileName(G4String);

    void SetSensLBining(G4ThreeVector);
    void SetSensRBining(G4ThreeVector);

    void SetAbsLBining(G4ThreeVector);
    void SetAbsRBining(G4ThreeVector);

    void SetHcalRBining(G4ThreeVector);

    void SetEcalResponse(G4ThreeVector);
    void SetEcalCellNoise(G4double);

    void SetJobRunNumber(G4int);

    G4int       GetnLtot()           {return nLtot;};
    G4int       GetnRtot()           {return nRtot;};
    G4double    GetdLbin()           {return dLbin;};
    G4double    GetdRbin()           {return dRbin;};

    G4int       GetAbsnLtot()        {return nLtotAbs;};
    G4int       GetAbsnRtot()        {return nRtotAbs;};
    G4double    GetAbsdLbin()        {return dLbinAbs;};
    G4double    GetAbsdRbin()        {return dRbinAbs;};

    G4int       GetHcalnRtot()       {return nRtotHcal;};
    G4double    GetHcaldRbin()       {return dRbinHcal;};

    G4int       GetJobRunNumber()    {return RunNumber;};
 
    G4String    GetfileName()        {return fileName;};

  private:

    TH1D*  histo[nhist];
    TH2D*  hits;

    TTree*    tree_tot;
    TTree*    tree_vec;
    TTree*    tree_ran;
    TTree*    tree_cell;

    G4double  EdepEcalRad, EdepEcalLong;
    G4double  EdepAbsRad, EdepAbsLong;
    G4double  EdepHcalRad, EdepHcalLong;
    G4double  EdepEcalHits;

    G4double  e_ecal, e_hcal, e_zero, e_abs;
    G4double  r_hcal, r_ecal;
    G4double  e_vec[17];

    G4int     n_cells;
    G4double  e_dep[25], e_phot[25], e_unif[25], e_eff[25];
    G4double  LightYield, LightCollEff, LightCollUnif, CellNoise;

    G4int    nLtot,  nRtot,  nLtotAbs,  nRtotAbs,  nRtotHcal;       
    G4double dLbin,  dRbin,  dLbinAbs,  dRbinAbs,  dRbinHcal;      

    G4int    RunNumber;

    G4String  fileName ;
    HistoMessenger* histoMessenger;
};


#endif
//===============================================================
