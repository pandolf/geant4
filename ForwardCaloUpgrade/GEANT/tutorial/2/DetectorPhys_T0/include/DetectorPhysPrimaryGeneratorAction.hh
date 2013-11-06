//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: DetectorPhysPrimaryGeneratorAction.hh,v 1.5 2002/01/09 17:24:11 ranjard Exp $
// GEANT4 tag $Name: geant4-04-01-patch-01 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorPhysPrimaryGeneratorAction_h
#define DetectorPhysPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class DetectorPhysDetectorConstruction;
class DetectorPhysPrimaryGeneratorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorPhysPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    DetectorPhysPrimaryGeneratorAction(DetectorPhysDetectorConstruction*);    
   ~DetectorPhysPrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event*);
    void SetRndmFlag(G4String val) { rndmFlag = val;}
    void SetUndulatorFlag(G4String val) { undulatorFlag = val;}

    G4double GetFirstParticleEnergy() {return firstParticleEneregy;};
//    G4double GetFirstParticleEnergy() {return 1972;};

    int ReadUndulator(char *, double *);
  private:
	
    G4double x ;
    G4double y;
    G4double z;

    G4double Und_Energ_Val[1000000];
    G4int Undulator_arrys;
    G4int Count0;

    G4double firstParticleEneregy;

    G4ParticleGun*                particleGun;	  //pointer a to G4  class
    DetectorPhysDetectorConstruction*    DetectorPhysDetector;  //pointer to the geometry
    
    DetectorPhysPrimaryGeneratorMessenger* gunMessenger; //messenger of this class

    G4String                      rndmFlag;	  //flag for a rndm impact point
    G4String                      undulatorFlag;
  G4double Ei;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


