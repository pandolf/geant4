#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#ifdef G4VIS_USE
#include "DetectorPhysVisManager.hh"
#endif

#include "DetectorPhysDetectorConstruction.hh"
#include "DetectorPhysPhysicsList.hh"
#include "DetectorPhysPrimaryGeneratorAction.hh"


namespace CLHEP {}
using namespace CLHEP; 


std::ofstream outFile_GasCollect ;
std::ofstream particle_out_Gas;

int main(int argc,char** argv) {

  // choose the Random engine
  HepRandom::setTheEngine(new RanecuEngine);

  // Construct the default run manager
  G4RunManager * runManager = new G4RunManager;

  // set mandatory initialization classes
  DetectorPhysDetectorConstruction* detector = new DetectorPhysDetectorConstruction;  
  runManager->SetUserInitialization(detector);


  G4VUserPhysicsList* the_physics = new DetectorPhysPhysicsList;
  runManager->SetUserInitialization(the_physics);  

  DetectorPhysPrimaryGeneratorAction *primarygeneration = new DetectorPhysPrimaryGeneratorAction(detector);
  runManager->SetUserAction(primarygeneration);


  //Initialize G4 kernel
  runManager->Initialize();


 G4UIsession* session=0;
  
 if (argc==1)   // Define UI session for interactive mode.
 {
     
#ifdef G4UI_USE_TCSH
     session = new G4UIterminal(new G4UItcsh);      
#else
     session = new G4UIterminal();
#endif

 }
 
#ifdef G4VIS_USE
 // visualization manager
  G4VisManager* visManager = new DetectorPhysVisManager;
  visManager->Initialize();
#endif



    
  // get the pointer to the User Interface manager 
  G4UImanager* UI = G4UImanager::GetUIpointer();  

  if (session)   // Define UI session for interactive mode.
    {
      UI->ApplyCommand("/control/execute vis_T0.mac");    
      session->SessionStart();
      delete session;
    }
  else           // Batch mode
    { 
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UI->ApplyCommand(command+fileName);
    }


#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  return 0;
}

