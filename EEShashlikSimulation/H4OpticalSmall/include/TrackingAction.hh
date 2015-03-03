#ifndef TrackingAction_H
#define TrackingAction_H 1

#include "G4UserTrackingAction.hh"
#include "TrackInformation.hh"


class TrackingAction : public G4UserTrackingAction
{

public:
  TrackingAction();
  ~TrackingAction();

public:
  void PreUserTrackingAction(const G4Track* aTrack);
  void PostUserTrackingAction(const G4Track* aTrack);

};

#endif
