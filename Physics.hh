#ifndef PHYSICS_HH
#define PHYSICS_HH

#include "G4VModularPhysicsList.hh"
//for Cherenkov light
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
//for general decay
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"

//implement a Physics class as a derived class from G4VModularPhysicsList
class MyPhysicsList : public G4VModularPhysicsList {
public:
  MyPhysicsList(); //default constructor
  ~MyPhysicsList(); //default destructor

};

#endif
