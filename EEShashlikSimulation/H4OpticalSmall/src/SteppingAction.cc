#include "SteppingAction.hh"
#include "common.h"
#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"

using namespace std;
using namespace CLHEP;

//G4double fibre0;
//G4double fibre1;
//G4double fibre2;
//G4double fibre3;

int to_int (string name)
{
  int Result ; // int which will contain the result
  stringstream convert (name) ;
  string dummy ;
  convert >> dummy ;
  convert >> Result ;
  return Result ;
}

//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

SteppingAction::SteppingAction (EEShashDetectorConstruction* detectorConstruction,
				const G4int& scint, const G4int& cher):
  fDetectorConstruction(detectorConstruction),
  propagateScintillation(scint),
  propagateCerenkov(cher)
{}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

SteppingAction::~SteppingAction ()
{}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void SteppingAction::UserSteppingAction (const G4Step * theStep)
{
  G4Track* theTrack = theStep->GetTrack () ;
  G4int trackID = theTrack->GetTrackID();
  TrackInformation* theTrackInfo = (TrackInformation*)(theTrack->GetUserInformation());
  G4ParticleDefinition* particleType = theTrack->GetDefinition () ;

  //Pre point = first step in a volume
  //PostStep point identifies step out of volume
  G4StepPoint * thePrePoint = theStep->GetPreStepPoint () ;
  //  G4StepPoint * thePostPoint = theStep->GetPostStepPoint () ;
  const G4ThreeVector & thePrePosition = thePrePoint->GetPosition () ;
  G4VPhysicalVolume * thePrePV = thePrePoint->GetPhysicalVolume () ;
  //  G4VPhysicalVolume * thePostPV = thePostPoint->GetPhysicalVolume () ;
  G4String thePrePVName = "" ; 
  if ( thePrePV ) thePrePVName = thePrePV -> GetName () ;
  //  G4String thePostPVName = "" ; 
  //  if ( thePostPV ) thePostPVName = thePostPV -> GetName () ;

  
  G4int nStep = theTrack -> GetCurrentStepNumber();
  G4TouchableHandle theTouchable = thePrePoint->GetTouchableHandle();
    
  //-------------
  // get position
  G4double global_x = thePrePosition.x()/mm;
  G4double global_y = thePrePosition.y()/mm;
  G4double global_z = thePrePosition.z()/mm;

  G4TransportationManager* transportMgr ; 
  transportMgr = G4TransportationManager::GetTransportationManager() ;
  G4PropagatorInField * fieldPropagator = transportMgr->GetPropagatorInField() ;
  if(nStep>70000){
    //       std::cout<<"mortacci nstep"<<nStep<<" particle:"<<particleType->GetParticleName()<<" volume:"<<theTrack->GetLogicalVolumeAtVertex()->GetName()<<" id:"<<trackID<<" position:"<<global_x<<" "<<global_y<<" "<<global_z<<" energy:"<<theTrack->GetTotalEnergy()/eV<<std::endl;
    theTrack->SetTrackStatus(fStopAndKill);
  }
  

  // optical photon
  if( particleType == G4OpticalPhoton::OpticalPhotonDefinition() )
    {
      
      G4int copyNo = theTouchable->GetCopyNumber();
      G4int motherCopyNo = theTouchable->GetCopyNumber(1);
      
      //FUCK IT Let's just kill them before they bounce that much...
      if( nStep>6 && theTrack->GetLogicalVolumeAtVertex()->GetName().contains("Act")){
	//	theTrack->SetTrackStatus(fKillTrackAndSecondaries);
	//	std::cout<<"mortacci Act"<<nStep<<" particle:"<<particleType->GetParticleName()<<" volume:"<<theTrack->GetLogicalVolumeAtVertex()->GetName()<<" id:"<<trackID<<" position:"<<global_x<<" "<<global_y<<" "<<global_z<<" energy:"<<theTrack->GetTotalEnergy()/eV<<std::endl;
	theTrack->SetTrackStatus(fStopAndKill);
      }

      G4String processName = theTrack->GetCreatorProcess()->GetProcessName();
      
      //----------------------------
      // count photons at production
      if( ( theTrack->GetLogicalVolumeAtVertex()->GetName().contains("Act") ) &&
	  (nStep == 1) && (processName == "Scintillation") )
	{
	  // CreateTree::Instance()->tot_phot_sci += 1;
	  // if( !propagateScintillation ) theTrack->SetTrackStatus(fKillTrackAndSecondaries);
	  //  fibre0 += 1; 
	}
      
      
      //----------------------------
      // count photons at fiber exit
      if(  thePrePVName.contains("Grease")&& copyNo==0 )	{
	EOpt_0+=theTrack->GetTotalEnergy()/eV;
	fibre0 += 1;
	//		std::cout<<EOpt_0<<std::endl;
	//	  CreateTree::Instance()->tot_gap_phot_sci += 1;
	// if you do not want to kill a photon once it exits the fiber, comment here below
	//  theTrack->SetTrackStatus(fKillTrackAndSecondaries);
      }
      
      if(  thePrePVName.contains("Grease")&& copyNo==1 )	{
	EOpt_1+=theTrack->GetTotalEnergy()/eV;
	fibre1 += 1;
	//	  CreateTree::Instance()->tot_gap_phot_sci += 1;
	// if you do not want to kill a photon once it exits the fiber, comment here below
	//  theTrack->SetTrackStatus(fKillTrackAndSecondaries);
      }
      
      if(  thePrePVName.contains("Grease")&& copyNo==2 )	{
	EOpt_2+=theTrack->GetTotalEnergy()/eV;
	fibre2 += 1;
	//	  CreateTree::Instance()->tot_gap_phot_sci += 1;
	// if you do not want to kill a photon once it exits the fiber, comment here below
	//  theTrack->SetTrackStatus(fKillTrackAndSecondaries);
      }

      if(  thePrePVName.contains("Grease")&& copyNo==3 )	{
	EOpt_3+=theTrack->GetTotalEnergy()/eV;
	fibre3 += 1;
	//	  CreateTree::Instance()->tot_gap_phot_sci += 1;
	// if you do not want to kill a photon once it exits the fiber, comment here below
	//  theTrack->SetTrackStatus(fKillTrackAndSecondaries);
      }
      
      
      
      if(  thePrePVName.contains("Grease") )
	{
	  //	  fibre2 += 1;
	}
      //------------------------------
      // count photons at the detector
      if(   thePrePVName.contains("Grease")&& motherCopyNo ==3 )  //same as if with the prePV == Fibre
	{
	  //	  fibre3 += 1;
	  // if you do not want to kill a photon once it enters the detector, comment here below
	  //  theTrack->SetTrackStatus(fKillTrackAndSecondaries);
	}
      
      
      /*
	if( (theTrack->GetLogicalVolumeAtVertex()->GetName().contains("core")) && (nStep == 1) )
	{
	//----------------------------------------------------------
	// storing time, energy and position at gap with fast timing
	Photon ph;
	ph.position.SetX(global_x);
	ph.position.SetY(global_y);
	ph.position.SetZ(global_z);
	ph.direction.SetX(theTrack->GetVertexMomentumDirection().x());
	ph.direction.SetY(theTrack->GetVertexMomentumDirection().y());
	ph.direction.SetZ(theTrack->GetVertexMomentumDirection().z());
	ph.dist = (global_z/(0.5*fiber_length));
	ph.energy = theTrack->GetTotalEnergy()/eV;
	Fiber* fib = fDetectorConstruction -> GetFiber();
	std::map<int,Travel> trc = GetTimeAndProbability(ph,fib,theTrackInfo->GetParticleProdTime());
	for(unsigned int it = 0; it < CreateTree::Instance()->attLengths->size(); ++it)
	{
	int attLength = int( CreateTree::Instance()->attLengths->at(it) );
	if( trc[attLength].prob[0] < 1.E-09 ) theTrack->SetTrackStatus(fKillTrackAndSecondaries);
	for(int it2 = 0; it2 < 3; ++it2)
	{
	CreateTree::Instance()->tot_gap_photFast_cer->at(it) += trc[attLength].prob[it2];
	//CreateTree::Instance()->h_photFast_cer_gap_lambda[attLength] -> Fill( MyMaterials::fromEvToNm(theTrack->GetTotalEnergy()/eV), trc[attLength].prob[it2] );
	//CreateTree::Instance()->h_photFast_cer_gap_E[attLength] -> Fill( theTrack->GetTotalEnergy()/eV, trc[attLength].prob[it2] );
	//CreateTree::Instance()->h_photFast_cer_gap_time[attLength] -> Fill( trc[attLength].time[it2], trc[attLength].prob[it2] );
	}
	}
	}
      */
    } // optical photon
  // non optical photon
  else
    {

      /*
      //G4cout << ">>> begin non optical photon" << G4endl;
      G4double energy = theStep->GetTotalEnergyDeposit() - theStep->GetNonIonizingEnergyDeposit();
      if ( energy == 0. ) return;
      //  CreateTree::Instance() -> depositedEnergyTotal += energy/GeV;
      if( thePrePVName.contains("core") )
	{
	  //	  CreateTree::Instance()->depositedEnergyCore += energy/GeV;
	}
      if( thePrePVName.contains("capillary") )
	{
	  //	  CreateTree::Instance()->depositedEnergyCapillary += energy/GeV;
	}
      if( thePrePVName.contains("cladding") )
	{
	  //	  CreateTree::Instance()->depositedEnergyCladding += energy/GeV;
	}
      if( thePrePVName.contains("world") )
	{
	  //	  CreateTree::Instance() -> depositedEnergyWorld += energy/GeV;
	}
      //G4cout << ">>> end non optical photon" << G4endl;
      */
    } // non optical photon
  return ;
}


