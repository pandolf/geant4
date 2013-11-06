//
// ********************************************************************
//
// ********************************************************************

#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "HistoManager.hh"
#include "G4Material.hh"
#include "G4Step.hh"
//#include "G4DynamicParticle.hh"
#include "Randomize.hh"

// Constructor determination with the pointers assignment
//
// detector(det)=> detector=det and the same for
// eventaction(evt) => eventaction=evt

SteppingAction::SteppingAction(DetectorConstruction* det,
                               EventAction* evt, HistoManager* hist)
:detector(det), eventaction(evt), histo(hist), oldEvtNumber(0)				 
{ }

// Distructor

SteppingAction::~SteppingAction()
{ }

// Determination of SteppingAction function

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{

// Geometrical informations are available from preStepPoint.
// G4VTouchable and its derivates keep these geometrical informations.
//---------------------------------------------------------------------
   G4TouchableHandle touch = aStep->GetPreStepPoint()->GetTouchableHandle();

// Get volume on the current step 
//-------------------------------- 
// GetPreStepPoint() - member of G4Step 
// GetVolume() - member of G4Track 
// GetTouchableHandle() - member of G4Track 
// G4VPhysicalVolume* volume 
// = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();

   G4VPhysicalVolume* volume = touch->GetVolume(); 

// Information on the current step 
//---------------------------------
   G4double edep = aStep->GetTotalEnergyDeposit(); 
   G4ThreeVector prePoint  = aStep->GetPreStepPoint()->GetPosition();
   G4ThreeVector postPoint = aStep->GetPostStepPoint()->GetPosition();
   G4double stepl  = aStep->GetStepLength();
   G4double charge = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
   G4double totalP = aStep->GetTrack()->GetDynamicParticle()->GetTotalMomentum();
   G4double totalE = aStep->GetTrack()->GetTotalEnergy();
   G4Material* mat = aStep->GetTrack()->GetMaterial();
   G4MaterialPropertiesTable* propertiesTable = mat->GetMaterialPropertiesTable();

// To get the copy number of the mother volume (layer number)
//         (Sens -> Gap -> Wrap -> Layer)
// ( depth = 0   ->  1  ->  2   ->   3  )
// G4int copyNumber = touch->GetCopyNumber(3);
//  
// Check, in which volume the present step is ? 
// GetEcal(), GetHcal() and GetZero() member functions of 
// DetectorConstruction-class which return pointer to physics volume
//
// AddEcal(edep), AddHcal(edep) and AddZero(edep) are member functions 
// of EventAction-class, which accumulate deposited energy

// collect information for Ecal sensitive media
//--------------------------------------------- 
   if( volume == detector->GetEcal()) 
   {
     G4int nEcalLayer = touch->GetCopyNumber(1);
     if( edep > 0. ) {
       G4ThreeVector aPoint  = prePoint + G4UniformRand()*(postPoint - prePoint);
       G4double radius = std::sqrt(aPoint.x()*aPoint.x()+aPoint.y()*aPoint.y());
       G4double offset = detector->GetEcalOffset();

       G4int SlideNb = int( (aPoint.z() - offset) / histo->GetdLbin() );
       if( SlideNb > histo->GetnLtot()   ) SlideNb = histo->GetnLtot();
       if( detector->GetNbOfEcalLayers() !=1) SlideNb = nEcalLayer; 

       G4int RingNb  = int( radius / histo->GetdRbin() );        
       if( RingNb > histo->GetnRtot() ) RingNb = histo->GetnRtot();

       G4double response = edep;
       G4double* birks = detector->GetEcalBirksConstant();
       G4double* birkL3 = detector->GetEcalBirkL3Constant();

       if(birks[0]*edep*stepl*charge != 0.) {
         G4double density = mat->GetDensity() / (g/cm3);
         G4double rkb     = birks[0]/density;
         if( int(birkL3[0]) > 0 )
           response = edep*getBirkL3(aStep, rkb, birkL3[1], birkL3[2]);
         else 
           response = edep*getAttenuation(aStep, rkb, birks[1], birks[2]);
       }

       eventaction->fillEcalStep(response,SlideNb,RingNb);
       eventaction->AddEcal(response);

// deposited energy in Ecal cells
//-------------------------------
       G4int    IndCell = G4int( sqrt(detector->GetNbOfEcalCells()) );
       G4double DxCell  = detector->GetEcalCellSize();
       G4double maxSize = 0.5*DxCell*IndCell;
       if( fabs(aPoint.x())<=maxSize && fabs(aPoint.y())<=maxSize) {
         G4int ix_Ind = int( fabs(-maxSize-aPoint.x()) / DxCell );
         G4int iy_Ind = int( fabs(-maxSize-aPoint.y()) / DxCell );
         G4int cell_Ind = ix_Ind + iy_Ind*IndCell;
         eventaction->fillEcalCell(cell_Ind,response);
       }

// hit point (Y vs Z) of first point in Ecal  
//------------------------------------------
       if( eventaction->GetEventNb() == oldEvtNumber) {
         oldEvtNumber = eventaction->GetEventNb() + 1;
         G4int    IndHit = histo->GetnRtot();
         G4double DxHits = histo->GetdRbin();
         G4double maxHit = 0.5*DxHits*IndHit;
         if( fabs(aPoint.x())<=maxHit && fabs(aPoint.y())<=maxHit ) {
           G4int ix_Hit = int( fabs(-maxHit-aPoint.x()) / DxHits );
           G4int iy_Hit = int( fabs(-maxHit-aPoint.y()) / DxHits );
           G4int hit_Ind = ix_Hit + iy_Hit*IndHit;
           eventaction->fillEcalHits(hit_Ind,response);
         }
       }

     }

     if( charge != 0.) {
       G4MaterialPropertyVector* Rindex = propertiesTable->GetProperty("RINDEX");
       //       G4double rind_ecal = Rindex->GetMaxProperty();
       G4double rind_ecal = Rindex->GetMaxValue();
       G4double beta = (totalE > 0.) ? totalP / totalE : 1.;
       if( beta*rind_ecal > 1.) eventaction->AddEcalRange(stepl,nEcalLayer); 
     }

   }

// collect information in Ecal absorber 
//-------------------------------------
   if( volume == detector->GetAbsEcal())
   {
     G4int nEcalLayer = touch->GetCopyNumber(1);
     if( edep > 0. ) {  
       G4ThreeVector aPoint  = prePoint + G4UniformRand()*(postPoint - prePoint);
       G4double radius = std::sqrt(aPoint.x()*aPoint.x()+aPoint.y()*aPoint.y());
       G4double offset = detector->GetEcalOffset();
       
       G4int SlideAbs = int( (aPoint.z() - offset) / histo->GetAbsdLbin() );
       if( SlideAbs > histo->GetAbsnLtot()  ) SlideAbs = histo->GetAbsnLtot();
       if( detector->GetNbOfEcalLayers() !=1) SlideAbs = nEcalLayer;
       G4int RingAbs  = int( radius / histo->GetAbsdRbin() );
       if( RingAbs > histo->GetAbsnRtot() ) RingAbs = histo->GetAbsnRtot();

       eventaction->fillAbsStep(edep,SlideAbs,RingAbs);
       eventaction->AddAbs(edep);
     }
   }

// collect Hcal information
//-------------------------- 
   if( volume == detector->GetHcal() ) {
     G4int nHcalLayer = touch->GetCopyNumber(3); 
     if( edep > 0. ) {
       G4ThreeVector aPoint  = prePoint + G4UniformRand()*(postPoint - prePoint);
       G4double radius = std::sqrt(aPoint.x()*aPoint.x()+aPoint.y()*aPoint.y());
       G4int RingHcal  = int( radius / histo->GetHcaldRbin() );        
       if( RingHcal > histo->GetHcalnRtot() ) RingHcal = histo->GetHcalnRtot();

       G4double response = edep;
       G4double* birks = detector->GetHcalBirksConstant();
       if(birks[0]*edep*stepl*charge != 0.) {
         G4double density = mat->GetDensity() / (g/cm3);
         G4double rkb     = birks[0]/density;
         response = edep*getAttenuation(aStep, rkb, birks[1], birks[2]);
       }

       eventaction->AddHcal(response);
       eventaction->fillHcalStep(response,nHcalLayer,RingHcal);
     }

     if( charge != 0.) {
       G4MaterialPropertyVector* Rindex = propertiesTable->GetProperty("RINDEX");
       //       G4double rind_hcal = Rindex->GetMaxProperty();
       G4double rind_hcal = Rindex->GetMaxValue();
       G4double beta = (totalE > 0.) ? totalP / totalE : 1.;
       if( beta*rind_hcal > 1.) eventaction->AddHcalRange(stepl,nHcalLayer);
     }
   }

   if(volume == detector->GetZero() && edep > 0.) {
     G4double response = edep;
     G4double* birks = detector->GetHcalBirksConstant();
     if(birks[0]*edep*stepl*charge != 0.) {
       G4double density = mat->GetDensity() / (g/cm3);
       G4double rkb     = birks[0]/density;
       response = edep*getAttenuation(aStep, rkb, birks[1], birks[2]);
     }
     eventaction->AddZero(response);
   }
 
//example of saving random number seed of this event, under condition
//// if (condition) G4RunManager::GetRunManager()->rndmSaveThisEvent(); 
}

// Example of Birk attenuation law in organic scintillators.
//-----------------------------------------------------------

G4double SteppingAction::getAttenuation(const G4Step* aStep, G4double rkb, 
                                            G4double birk2, G4double birk3)
{
   G4double weight  = 1.;
   G4double destep  = aStep->GetTotalEnergyDeposit();
   G4double stepl   = aStep->GetStepLength();  
   G4double charge  = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
   G4double dedx    = destep/(stepl / cm); 
   G4double c       = birk2*rkb*rkb;

   if( fabs(charge) >= 2. && birk3 !=0. ) rkb /= birk3;
   weight = 1./(1.+rkb*dedx+c*dedx*dedx); 

   return weight;
}

G4double SteppingAction::getBirkL3(const G4Step* aStep, G4double rkb, 
                                 G4double birkSlope, G4double birkCut)
{    
   G4double weight  = 1.;
   G4double destep  = aStep->GetTotalEnergyDeposit();
   G4double stepl   = aStep->GetStepLength();
   G4double dedx    = destep/(stepl / cm);

   if( dedx > 0 ) {
      weight         = 1. - birkSlope*log(rkb*dedx);
      if (weight < birkCut) weight = birkCut;
      else if (weight > 1.) weight = 1.;
   }       
           
   return weight;
}      

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
