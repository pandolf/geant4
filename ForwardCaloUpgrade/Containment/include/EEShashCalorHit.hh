//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: EEShashCalorHit.hh 69223 2013-04-23 12:36:10Z gcosmo $
//
/// \file EEShashCalorHit.hh
/// \brief Definition of the EEShashCalorHit class

#ifndef EEShashCalorHit_h
#define EEShashCalorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

/// Calorimeter hit class
///
/// It defines data members to store the the energy deposit and track lengths
/// of charged particles in a selected volume:
/// - fEdep, fTrackLength

class EEShashCalorHit : public G4VHit
{
  public:
    EEShashCalorHit();
    EEShashCalorHit(const EEShashCalorHit&);
    virtual ~EEShashCalorHit();

    // operators
    const EEShashCalorHit& operator=(const EEShashCalorHit&);
    G4int operator==(const EEShashCalorHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw() {}
    virtual void Print();

    // methods to handle data
    void Add(G4double de, G4double dl);

    // get methods
    G4double GetEdep() const;
    G4double GetTrackLength() const;
      
  private:
    G4double fEdep;        ///< Energy deposit in the sensitive volume
    G4double fTrackLength; ///< Track length in the  sensitive volume
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<EEShashCalorHit> EEShashCalorHitsCollection;

extern G4ThreadLocal G4Allocator<EEShashCalorHit>* EEShashCalorHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* EEShashCalorHit::operator new(size_t)
{
  if(!EEShashCalorHitAllocator)
    EEShashCalorHitAllocator = new G4Allocator<EEShashCalorHit>;
  void *hit;
  hit = (void *) EEShashCalorHitAllocator->MallocSingle();
  return hit;
}

inline void EEShashCalorHit::operator delete(void *hit)
{
  if(!EEShashCalorHitAllocator)
    EEShashCalorHitAllocator = new G4Allocator<EEShashCalorHit>;
  EEShashCalorHitAllocator->FreeSingle((EEShashCalorHit*) hit);
}

inline void EEShashCalorHit::Add(G4double de, G4double dl) {
  fEdep += de; 
  fTrackLength += dl;
}

inline G4double EEShashCalorHit::GetEdep() const { 
  return fEdep; 
}

inline G4double EEShashCalorHit::GetTrackLength() const { 
  return fTrackLength; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
