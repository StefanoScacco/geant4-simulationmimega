#ifndef GENERATOR_HH
#define GENERATOR_HH

#include "G4VUserPrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh" //class of all particles to generate
#include "G4Geantino.hh"
#include "G4IonTable.hh"

//new general generator of particles with many more commands
#include "G4GeneralParticleSource.hh"

//inherit from this fundamental class
class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction {
public:
  MyPrimaryGenerator();
  ~MyPrimaryGenerator();

  virtual void GeneratePrimaries(G4Event*); //fundamental function to implement particle generation

private:
  G4GeneralParticleSource* fParticleGun; //this will be our particle source
};

#endif
