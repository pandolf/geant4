#ifndef TrackInformation_h
#define TrackInformation_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4Allocator.hh"
#include "G4VUserTrackInformation.hh"
#include "EEShashDetectorConstruction.hh"

class TrackInformation : public G4VUserTrackInformation
{
public:
  TrackInformation();
  TrackInformation(const G4Track* aTrack);
  TrackInformation(const TrackInformation* aTrackInfo);
  virtual ~TrackInformation();
  inline void *operator new(size_t);
  inline void operator delete(void *aTrackInfo);
  inline int operator ==(const TrackInformation& right) const {return (this==&right);}
  void Print() const;


private:
  G4ParticleDefinition* particleDefinition;
  G4String particleName;
  G4int particleTrackID;
  G4ThreeVector particlePosition;
  G4ThreeVector particleMomentum;
  G4double particleEnergy;
  G4double particleProdTime;
  G4double particleTime;
  G4ParticleDefinition* parentDefinition;
  G4String parentName;
  G4int parentTrackID;
  G4ThreeVector parentPosition;
  G4ThreeVector parentMomentum;
  G4double parentEnergy;
  G4double parentTime;


public:
  inline G4ParticleDefinition* GetParticleDefinintion() const { return particleDefinition; };
  inline G4String GetParticleName() const { return particleName; };
  inline G4int GetParticleTrackID() const { return particleTrackID; };
  inline G4ThreeVector GetParticlePosition() const { return particlePosition; };
  inline G4ThreeVector GetParticleMomentum() const { return particleMomentum; };
  inline G4double GetParticleEnergy() const { return particleEnergy; };
  inline G4double GetParticleProdTime() const { return particleProdTime; };
  inline G4double GetParticleTime() const { return particleTime; };
  inline G4ParticleDefinition* GetParentDefinintion() const { return parentDefinition; };
  inline G4String GetParentName() const { return parentName; };
  inline G4int GetParentTrackID() const { return parentTrackID; };
  inline G4ThreeVector GetParentPosition() const { return parentPosition; };
  inline G4ThreeVector GetParentMomentum() const { return parentMomentum; };
  inline G4double GetParentEnergy() const { return parentEnergy; };
  inline G4double GetParentTime() const { return parentTime; };
  inline void SetParticleProdTimeInformation(const G4double& prodTime) { particleProdTime = prodTime; };
  void SetParticleInformation(const TrackInformation* aTrackInfo);
  void SetParentInformation(const TrackInformation* aTrackInfo);
};


extern G4Allocator<TrackInformation> aTrackInformationAllocator;
inline void* TrackInformation::operator new(size_t)
{
  void* aTrackInfo;
  aTrackInfo = (void*)aTrackInformationAllocator.MallocSingle();
  return aTrackInfo;
}
inline void TrackInformation::operator delete(void *aTrackInfo)
{
  aTrackInformationAllocator.FreeSingle((TrackInformation*)aTrackInfo);
}


#endif
