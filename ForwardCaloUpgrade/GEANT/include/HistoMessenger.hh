//
// ********************************************************************

#ifndef HistoMessenger_h
#define HistoMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class HistoManager;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWith3Vector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoMessenger: public G4UImessenger
{
  public:
    HistoMessenger(HistoManager* );
   ~HistoMessenger();

    void SetNewValue(G4UIcommand*, G4String);

  private:

    HistoManager*              histoManager;

    G4UIdirectory*             testDir;
    G4UIdirectory*             histoDir;
    G4UIcmdWithAString*        FileNameCmd;
    G4UIcmdWith3Vector*        LBinEcalCmd;
    G4UIcmdWith3Vector*        RBinEcalCmd;
    G4UIcmdWith3Vector*        LBinAbsCmd;
    G4UIcmdWith3Vector*        RBinAbsCmd;
    G4UIcmdWith3Vector*        RBinHcalCmd;
    G4UIcmdWith3Vector*        RespEcalCmd;
    G4UIcmdWithADoubleAndUnit* NoiseEcalCmd;
    G4UIcmdWithAnInteger*      RunNumberCmd;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

