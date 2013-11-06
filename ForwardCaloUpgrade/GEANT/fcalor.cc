//
// *********************************************************
// example of the present HE geometry with the
// ECAL in front of and the different dead material
// The different physics list (hadronic model) is 
// determined in this place 
//
// **********************************************************
//

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"
#include "time.h"

// user application
// -----------------
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"      
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "HistoManager.hh"

//#include "QGSP_BERT.hh"
#include "QGSP_BERT_EMV.hh"
//#include "QGSP_FTFP_BERT.hh"
//#include "FTFP_BERT_EMV.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#if defined(G4UI_USE_TCSH)
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#elif defined(G4UI_USE_XM)
#include "G4UIXm.hh"
#elif defined(G4UI_USE_WIN32)
#include "G4UIWin32.hh"
#elif defined(G4UI_USE_QT)
#include "G4UIQt.hh"
#include "G4Qt.hh"
#else
#include "G4UIterminal.hh"
#endif


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
// Choose the Random engine

  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);

// Set random seed with system time

  G4long seed = time(NULL);
  CLHEP::HepRandom::setTheSeed(seed);
  
// Construct the default run manager, which manages start
// and stop simulation
  
  G4RunManager * runManager = new G4RunManager;

// Set mandatory initialization classes:
// =====================================

// Initilization detector construction class
// -----------------------------------------
  DetectorConstruction* detector = new DetectorConstruction;
  runManager->SetUserInitialization(detector);

// Initilization of the different physics list
// --------------------------------------------

//  runManager-> SetUserInitialization(new QGSP_BERT);
  runManager-> SetUserInitialization(new QGSP_BERT_EMV);
//  runManager-> SetUserInitialization(new QGSP_FTFP_BERT);
//  runManager-> SetUserInitialization(new FTFP_BERT_EMV);

// Initilization of histograms 
//-----------------------------
  HistoManager* histo = new HistoManager();
 
// Set user gen-action class
// --------------------------  	
  PrimaryGeneratorAction* gen_action = 
                          new PrimaryGeneratorAction(detector);
  runManager->SetUserAction(gen_action);

// Set user run-action class
// -------------------------
  RunAction* run_action = new RunAction(detector, gen_action, histo);  
  runManager->SetUserAction(run_action);


// Set user event-action class
// ---------------------------
  EventAction* event_action = new EventAction(run_action, detector, histo);
  runManager->SetUserAction(event_action);

// Set user stepping-action class
// ------------------------------
  G4UserSteppingAction* stepping_action =
                    new SteppingAction(detector, event_action, histo);
  runManager->SetUserAction(stepping_action);
  
// Initialize G4 kernel

  runManager->Initialize();
  
#ifdef G4VIS_USE
  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif
  
// Get the pointer to the User Interface manager

  G4UImanager* UI = G4UImanager::GetUIpointer();      
  
  if (argc!=1)   // batch mode
    {
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UI->ApplyCommand(command+fileName);    
    }
  else           // interactive mode : define visualization UI terminal
    {
      G4UIsession* session = 0;
#if defined(G4UI_USE_TCSH)
      session = new G4UIterminal(new G4UItcsh);      
#elif defined(G4UI_USE_XM)
      session = new G4UIXm(argc,argv);
      UI->ApplyCommand("/control/execute visTutor/gui.mac");      
#elif defined(G4UI_USE_WIN32)
      session = new G4UIWin32();
      UI->ApplyCommand("/control/execute visTutor/gui.mac");      
#elif defined(G4UI_USE_QT)
      session = new G4UIQt(argc,argv);
      UI->ApplyCommand("/control/execute visTutor/gui.mac");      
#else
      session = new G4UIterminal();
#endif
#ifdef G4VIS_USE
      UI->ApplyCommand("/control/execute vis.mac");     
#endif
      session->SessionStart();
      delete session;
    }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !
#ifdef G4VIS_USE
  delete visManager;
#endif                
  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
