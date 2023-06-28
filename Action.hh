#ifndef ACTION_HH
#define ACTION_HH

#include "G4VUserActionInitialization.hh"
#include "Generator.hh" //must be included from the other file to create the action
//inherit from this fundamental class

#include "Run.hh" //need for storing data in ROOT file 
#include "Event.hh" //initialize events and steps
#include "Stepping.hh" 

class MyActionInitialization : public G4VUserActionInitialization {
public:
  MyActionInitialization();
  ~MyActionInitialization();

  virtual void Build() const; //fundamental function to implement every step of the simulation

};

#endif
