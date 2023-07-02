#include "Stepping.hh"

MySteppingAction::MySteppingAction(MyEventAction *eventAction) {
  fEventAction = eventAction; //our event class instance is assigned
}

MySteppingAction::~MySteppingAction() {
}

void MySteppingAction::UserSteppingAction(const G4Step *Step) {
  //that is why we included the DetectorConstruction class
  G4LogicalVolume *volume = Step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume(); //this is all to get the logical volume of the step in which we simulate our action

  //this is how we access the scoring volume of our geometry: it is the radiator
  const MyDetectorConstruction *detectorConstruction = static_cast<const MyDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  G4LogicalVolume *fScoringVolume = detectorConstruction->GetScoringVolume();

  //now we check: if the step volume in which simulation occurs is NOT the scoring volume, we get no information about the energy!
  if(volume != fScoringVolume)
    return;

  //all we have to do is get the energy from the step class and sum it over all possible steps to form the single event, so we can hand the total energy over to the Event class, and multiply all the copy numbers from detector
  G4double edep = Step->GetTotalEnergyDeposit();
  //  G4cout << "Step energy deposit: " << edep << G4endl;
  G4double mass = Step->GetPreStepPoint()->GetMass();
  G4double M_thresh = 100*MeV;
  G4int copyall = 1;
  if (mass > M_thresh) { //if it is a muon, we multiply also together all the copy numbers
    copyall = Step->GetPreStepPoint()->GetTouchable()->GetCopyNumber();
    fEventAction->MultiplyCopyall(copyall); //multiplies all copy numbers
  }
  
  fEventAction->AddEdep(edep); //adds total energy. In vacuum it should be 0
}

  
