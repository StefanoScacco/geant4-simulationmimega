#ifndef STEPPING_HH
#define STEPPING_HH

//stepping for the event
#include "G4UserSteppingAction.hh" 
#include "G4Step.hh"

#include "DetectorConstruction.hh" //needed 
#include "Event.hh"

class MySteppingAction : public G4UserSteppingAction {
public:
  MySteppingAction(MyEventAction* eventAction);
  ~MySteppingAction();

  virtual void UserSteppingAction(const G4Step*); //built-in in the G4UserSteppingAction class

private:
  MyEventAction *fEventAction; //add the energy deposition to the variable event
};

#endif
