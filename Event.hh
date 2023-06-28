#ifndef EVENT_HH
#define EVENT_HH

//this class implements single event to study
#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "Run.hh"

class MyEventAction : public G4UserEventAction{
public:
  MyEventAction(MyRunAction*);
  ~MyEventAction();

  //functions for G4UserEventAction
  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

  void AddEdep(G4double edep) { fEdep+=edep; } //accumulates energy
  void MultiplyCopyall(G4int copyall) { fCopyallHit*=copyall; } //multiplies copy numbers

  G4double effG1N(G4double Vpot) { return 0.97678/(1 + exp(-0.06211 * (Vpot - 1755.01) ) ); }
  G4double effG1B(G4double Vpot) { return 0.95598/(1 + exp(-0.05005 * (Vpot - 1815.88) ) ); }
  G4double effP2(G4double Vpot) { return 0.92522/(1 + exp(-0.022 * (Vpot - 1541.97) ) ); }
  G4double effP3(G4double Vpot) { return 0.92585/(1 + exp(-0.0769 * (Vpot - 1520.42) ) ); }
  //these are the data for the P2 scintillator efficiency. The function was obtained experimentally by fitting the efficiency. Vpot must be given in Volts
  
private:
  G4double Eff[4]; //for the efficiencies
  G4double fEdep; //to store energy deposited
  G4int fCopyallHit, fCopyallSeen; //to store product of coincidences hit and seen
  G4int ID; //ID of the event to see at what point of the simulation we are at
};

#endif
