//
// ********************************************************************
// ********************************************************************
//

#include "HistoMessenger.hh"

#include <sstream>

#include "HistoManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3Vector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoMessenger::HistoMessenger(HistoManager * man)
:histoManager(man)
{ 
  testDir = new G4UIdirectory("/test/");
  testDir->SetGuidance("UI commands specific to this example");
  
  histoDir = new G4UIdirectory("/test/histo/");
  histoDir->SetGuidance("histograms control");

  FileNameCmd = new G4UIcmdWithAString("/test/histo/setRootName",this);
  FileNameCmd->SetGuidance("set file name for ROOT output");
  FileNameCmd->SetParameterName("choice",false);
  FileNameCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  LBinEcalCmd = new G4UIcmdWith3Vector("/test/histo/setSensLbin",this);
  LBinEcalCmd->SetGuidance("set longitudinal bining");
  LBinEcalCmd->SetGuidance("nb of bins; bin thickness (in mm)");
  LBinEcalCmd->SetParameterName("nLtot","dLbin"," ",true);
  LBinEcalCmd->SetRange("nLtot>=1 && dLbin>0");
  LBinEcalCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  RBinEcalCmd = new G4UIcmdWith3Vector("/test/histo/setSensRbin",this);
  RBinEcalCmd->SetGuidance("set radial bining");
  RBinEcalCmd->SetGuidance("nb of bins; bin thickness (in mm)");
  RBinEcalCmd->SetParameterName("nRtot","dRbin"," ",true);
  RBinEcalCmd->SetRange("nRtot>=1 && dRbin>0");
  RBinEcalCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  LBinAbsCmd = new G4UIcmdWith3Vector("/test/histo/setAbsLbin",this);
  LBinAbsCmd->SetGuidance("set longitudinal bining");
  LBinAbsCmd->SetGuidance("nb of bins; bin thickness (in mm)");
  LBinAbsCmd->SetParameterName("nLtotA","dLbinA"," ",true);
  LBinAbsCmd->SetRange("nLtotA>=1 && dLbinA>0");
  LBinAbsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  RBinAbsCmd = new G4UIcmdWith3Vector("/test/histo/setAbsRbin",this);
  RBinAbsCmd->SetGuidance("set radial bining");
  RBinAbsCmd->SetGuidance("nb of bins; bin thickness (in mm)");
  RBinAbsCmd->SetParameterName("nRtotA","dRbinA"," ",true);
  RBinAbsCmd->SetRange("nRtotA>=1 && dRbinA>0"); 
  RBinAbsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  RBinHcalCmd = new G4UIcmdWith3Vector("/test/histo/setHcalRbin",this);
  RBinHcalCmd->SetGuidance("set radial bining");
  RBinHcalCmd->SetGuidance("nb of bins; bin thickness (in mm)");
  RBinHcalCmd->SetParameterName("nRtotH","dRbinH"," ",true);
  RBinHcalCmd->SetRange("nRtotH>=1 && dRbinH>0");
  RBinHcalCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  RespEcalCmd = new G4UIcmdWith3Vector("/test/histo/setEcalResponse",this);
  RespEcalCmd->SetGuidance("set Ecal resonse");
  RespEcalCmd->SetGuidance("PhotoStat; LightCollEff; LightCollUnif");
  RespEcalCmd->SetParameterName("LYield","LEff","LUnif",true);
  RespEcalCmd->SetRange("LYield>=0 && LEff>=0 && LUnif>=0");
  RespEcalCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  NoiseEcalCmd = new G4UIcmdWithADoubleAndUnit("/test/histo/setEcalCellNoise",this);
  NoiseEcalCmd->SetGuidance("Set Noise for Ecal cells");
  NoiseEcalCmd->SetParameterName("Noise",false);
  NoiseEcalCmd->SetRange("Noise>=0.0");
  NoiseEcalCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  RunNumberCmd = new G4UIcmdWithAnInteger("/test/histo/setRunNumber",this);
  RunNumberCmd->SetGuidance("Set Run Number for this job");
  RunNumberCmd->SetParameterName("Run",false);
  RunNumberCmd->SetRange("Run>0");
  RunNumberCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoMessenger::~HistoMessenger()
{
  
  delete LBinEcalCmd;
  delete RBinEcalCmd;
  delete LBinAbsCmd;
  delete RBinAbsCmd;
  delete RBinHcalCmd;
  delete RespEcalCmd;
  delete NoiseEcalCmd;
  delete RunNumberCmd;
  delete FileNameCmd;  
  delete histoDir;
  delete testDir;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if( command == FileNameCmd )
   { histoManager->SetFileName(newValue);}

  if( command == LBinEcalCmd )
   { histoManager->SetSensLBining(LBinEcalCmd->GetNew3VectorValue(newValue));}

  if( command == RBinEcalCmd ) 
   { histoManager->SetSensRBining(RBinEcalCmd->GetNew3VectorValue(newValue));}    

  if( command == LBinAbsCmd )
   { histoManager->SetAbsLBining(LBinAbsCmd->GetNew3VectorValue(newValue));}
 
  if( command == RBinAbsCmd )
   { histoManager->SetAbsRBining(RBinAbsCmd->GetNew3VectorValue(newValue));}

  if( command == RBinHcalCmd ) 
   { histoManager->SetHcalRBining(RBinHcalCmd->GetNew3VectorValue(newValue));}    

  if( command == RespEcalCmd )
   { histoManager->SetEcalResponse(RespEcalCmd->GetNew3VectorValue(newValue));}

  if( command == NoiseEcalCmd )
   { histoManager->SetEcalCellNoise(NoiseEcalCmd->GetNewDoubleValue(newValue));}

  if( command == RunNumberCmd )
   { histoManager->SetJobRunNumber(RunNumberCmd->GetNewIntValue(newValue));}
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
