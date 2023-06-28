#ifndef RUN_HH
#define RUN_HH

#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh" //built-in functions root functions for geant4, no need to install dependencies
//#include "g4root.hh" NOT WORKING 
#include "G4SystemOfUnits.hh"
#include "G4Run.hh" //necessary to implement a run in Run.cc

//store data in ROOT file
class MyRunAction : public G4UserRunAction {
public:
  MyRunAction();
  ~MyRunAction();

  //functions to implement beginning and end of run action to store in ROOT file
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

};

#endif
