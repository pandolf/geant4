#include "TrackingAction.hh"

//#include "CreateTree.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"
#include "G4TrackingManager.hh"

using namespace CLHEP;

TrackingAction::TrackingAction()
{}

TrackingAction::~TrackingAction()
{}


void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  
  //---------------------
  // tracking information
  
  if( aTrack->GetUserInformation() == 0 )
    {
      TrackInformation* aTrackInfo = new TrackInformation(aTrack);
      G4Track* theTrack = (G4Track*)aTrack;
      theTrack->SetUserInformation( aTrackInfo );
    }
  else
    {
      TrackInformation* oldTrackInfo = (TrackInformation*)(aTrack->GetUserInformation());
      TrackInformation* aTrackInfo = new TrackInformation(aTrack);
      oldTrackInfo -> SetParticleInformation( aTrackInfo );
      G4Track* theTrack = (G4Track*)aTrack;
      theTrack -> SetUserInformation( oldTrackInfo );
   
      delete aTrackInfo;
    }
}


void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
  TrackInformation* aTrackInfo = (TrackInformation*)( aTrack->GetUserInformation() );
  G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
  if( secondaries )
    {
      for(unsigned int i = 0; i < secondaries->size(); ++i)
	{
	  G4Track* secTrack = (*secondaries)[i];
	  TrackInformation* newTrackInfo = new TrackInformation((*secondaries)[i]);
	  newTrackInfo -> SetParentInformation( aTrackInfo );
	  newTrackInfo -> SetParticleProdTimeInformation( secTrack->GetGlobalTime()/picosecond );
	  secTrack -> SetUserInformation( newTrackInfo );
	}
    }

  if( (aTrackInfo->GetParticleName() == "gamma" && aTrackInfo->GetParentName() == "pi0") ||
      (aTrackInfo->GetParticleName() == "e-" && aTrackInfo->GetParentName() == "pi0") ||
      (aTrackInfo->GetParticleName() == "e+" && aTrackInfo->GetParentName() == "pi0") ||
      (aTrackInfo->GetParticleName() == "gamma" && aTrackInfo->GetParentName() == "eta") ||
      (aTrackInfo->GetParticleName() == "e-" && aTrackInfo->GetParentName() == "eta") ||
      (aTrackInfo->GetParticleName() == "e+" && aTrackInfo->GetParentName() == "eta") )
    {
      ;
      // CreateTree::Instance()->Total_em_energy += aTrackInfo->GetParticleEnergy()/GeV;
    }
  
}
