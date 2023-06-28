#ifndef DETECTOR_HH
#define DETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh" //need to include that to save data in output file
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicsVector.hh" //useful to store quantum efficiency in

//class for sensitive detector that inherits from a geant4 class
class MySensitiveDetector : public G4VSensitiveDetector {
public:
  MySensitiveDetector(G4String name); //must be name, string associated to which scintillator it is, and supply voltage
  ~MySensitiveDetector();

private:
  //main function in this class that process events
  virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);

  //we want to implement detector efficiency for photons
  G4PhysicsFreeVector *quEff; //this class implements efficiency data on the detector

  //we want to implement detector efficiency for muons. This is done in Event.hh and Event.cc. Here is not needed
  
  
};

#endif
