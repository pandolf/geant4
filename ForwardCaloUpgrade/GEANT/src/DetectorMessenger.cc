//
// ********************************************************************
// ********************************************************************
//

#include "DetectorMessenger.hh"

#include <sstream>

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3Vector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction * Det)
:Detector(Det)
{ 
  ecalDir = new G4UIdirectory("/ecal/");
  ecalDir->SetGuidance("UI commands specific to this example");
  
  detDir = new G4UIdirectory("/ecal/det/");
  detDir->SetGuidance("detector construction commands");

  NbLayersCmd = new G4UIcmdWithAnInteger("/ecal/det/setNbOfLayers",this);
  NbLayersCmd->SetGuidance("Set number of Ecal layers.");
  NbLayersCmd->SetParameterName("NbLayers",false);
  NbLayersCmd->SetRange("NbLayers>0");
  NbLayersCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  HcalAbsMaterCmd = new G4UIcmdWithAString("/ecal/det/setHcalAbsMat",this);
  HcalAbsMaterCmd->SetGuidance("Select Material of the Hcal Absorber.");
  HcalAbsMaterCmd->SetParameterName("choice",false);
  HcalAbsMaterCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  HcalSensMaterCmd = new G4UIcmdWithAString("/ecal/det/setHcalSensMat",this);
  HcalSensMaterCmd->SetGuidance("Select Sensitive Material of Hcal");
  HcalSensMaterCmd->SetParameterName("choice",false);
  HcalSensMaterCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  EcalAbsMaterCmd = new G4UIcmdWithAString("/ecal/det/setEcalAbsMat",this);
  EcalAbsMaterCmd->SetGuidance("Select Material of the Ecal Absorber.");
  EcalAbsMaterCmd->SetParameterName("choice",false);
  EcalAbsMaterCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  EcalAbsThickCmd = new G4UIcmdWithADoubleAndUnit("/ecal/det/setEcalAbsThick",this);
  EcalAbsThickCmd->SetGuidance("Set Thickness of the Ecal Absorber");
  EcalAbsThickCmd->SetParameterName("Size",false);
  EcalAbsThickCmd->SetRange("Size>=0. && Size<=400.");
  EcalAbsThickCmd->SetUnitCategory("Length");
  EcalAbsThickCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  EcalSensMaterCmd = new G4UIcmdWithAString("/ecal/det/setEcalSensMat",this);
  EcalSensMaterCmd->SetGuidance("Select Sensitive Material of Ecal");
  EcalSensMaterCmd->SetParameterName("choice",false);
  EcalSensMaterCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  EcalSensThickCmd = new G4UIcmdWithADoubleAndUnit("/ecal/det/setEcalSensThick",this);
  EcalSensThickCmd->SetGuidance("Set Thickness of the Ecal Sensitive");
  EcalSensThickCmd->SetParameterName("Size",false);
  EcalSensThickCmd->SetRange("Size>=0.0 && Size<=400.");
  EcalSensThickCmd->SetUnitCategory("Length");  
  EcalSensThickCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  UpdateCmd = new G4UIcmdWithoutParameter("/ecal/det/update",this);
  UpdateCmd->SetGuidance("Update calorimeter geometry.");
  UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  UpdateCmd->SetGuidance("if you changed geometrical value(s).");
  UpdateCmd->AvailableForStates(G4State_Idle);

  MagFieldCmd = new G4UIcmdWithADoubleAndUnit("/ecal/det/setMagField",this);  
  MagFieldCmd->SetGuidance("Define magnetic field.");
  MagFieldCmd->SetGuidance("Magnetic field will be in Z direction.");
  MagFieldCmd->SetParameterName("Bz",false);
  MagFieldCmd->SetUnitCategory("Magnetic flux density");
  MagFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  

  NbCellEcalCmd = new G4UIcmdWith3Vector("/ecal/det/setEcalCells",this);
  NbCellEcalCmd->SetGuidance("set number and transverse size Ecal cell");
  NbCellEcalCmd->SetGuidance("nb of cells; transverse cell size [mm]");
  NbCellEcalCmd->SetParameterName("nCells","dxCell"," ",true);
  NbCellEcalCmd->SetRange("nCells>=1 && nCells < 26 && dxCell>0");
  NbCellEcalCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  BirksConsHcalCmd = new G4UIcmdWith3Vector("/ecal/det/setHcalBirks",this);
  BirksConsHcalCmd->SetGuidance("set Hcal Birks constant");
  BirksConsHcalCmd->SetGuidance("birk1; birk2; birk3");
  BirksConsHcalCmd->SetParameterName("birk1","birk2","birk3",true);
  BirksConsHcalCmd->SetRange("birk1>=0 && birk2>=0 && birk3>=0");
  BirksConsHcalCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  BirksConsEcalCmd = new G4UIcmdWith3Vector("/ecal/det/setEcalBirks",this);
  BirksConsEcalCmd->SetGuidance("set Ecal Birks constant");
  BirksConsEcalCmd->SetGuidance("birk1; birk2; birk3");
  BirksConsEcalCmd->SetParameterName("birk1","birk2","birk3",true);
  BirksConsEcalCmd->SetRange("birk1>=0 && birk2>=0 && birk3>=0");
  BirksConsEcalCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  BirkL3ConsEcalCmd = new G4UIcmdWith3Vector("/ecal/det/setEcalBirkL3",this);
  BirkL3ConsEcalCmd->SetGuidance("set Ecal Birk L3 constants");
  BirkL3ConsEcalCmd->SetGuidance("birk1; birk2; birk3");
  BirkL3ConsEcalCmd->SetParameterName("birk1","birk2","birk3",true);
  BirkL3ConsEcalCmd->SetRange("birk1>=0 && birk2>=0 && birk3>=0");
  BirkL3ConsEcalCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete NbLayersCmd;
  delete EcalAbsMaterCmd;
  delete EcalAbsThickCmd; 
  delete EcalSensMaterCmd;
  delete EcalSensThickCmd;
  delete HcalAbsMaterCmd;
  delete HcalSensMaterCmd;
  delete MagFieldCmd;
  delete NbCellEcalCmd;
  delete BirksConsHcalCmd;
  delete BirksConsEcalCmd;
  delete BirkL3ConsEcalCmd;
  delete UpdateCmd;
  delete detDir;
  delete ecalDir;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if( command == NbLayersCmd )
   { Detector->SetNbOfEcalLayers(NbLayersCmd->GetNewIntValue(newValue));}

  if( command == HcalAbsMaterCmd )
   { Detector->SetHcalAbsMaterial(newValue);}

  if( command == HcalSensMaterCmd )
   { Detector->SetHcalSensMaterial(newValue);}

  if( command == EcalSensMaterCmd )
   { Detector->SetEcalSensMaterial(newValue);}

  if( command == EcalAbsMaterCmd )
   { Detector->SetEcalAbsMaterial(newValue);}

  if( command == EcalAbsThickCmd )
   { Detector->SetEcalAbsThickness(EcalAbsThickCmd->GetNewDoubleValue(newValue));}
   
  if( command == EcalSensThickCmd )
   { Detector->SetEcalSensThickness(EcalSensThickCmd->GetNewDoubleValue(newValue));}

  if( command == MagFieldCmd )
   { Detector->SetMagField(MagFieldCmd->GetNewDoubleValue(newValue));}
   
  if( command == NbCellEcalCmd )
   { Detector->SetEcalCells(NbCellEcalCmd->GetNew3VectorValue(newValue));}
   
  if( command == BirksConsHcalCmd )
   { Detector->SetHcalBirksConstant(BirksConsHcalCmd->GetNew3VectorValue(newValue));}
   
  if( command == BirksConsEcalCmd )
   { Detector->SetEcalBirksConstant(BirksConsEcalCmd->GetNew3VectorValue(newValue));}
   
  if( command == BirkL3ConsEcalCmd )
   { Detector->SetEcalBirkL3Constant(BirkL3ConsEcalCmd->GetNew3VectorValue(newValue));}
   
  if( command == UpdateCmd )
   { Detector->UpdateGeometry(); }
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
