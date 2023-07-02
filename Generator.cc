#include "Generator.hh"

MyPrimaryGenerator::MyPrimaryGenerator() {
  /* in Geant4 you could either have events with more than 1 particle but it is better to have single particle single events and then combine them together to have a run */
  fParticleGun = new G4GeneralParticleSource();

  /*in Geant4, if we want to change particles types and momentum, we need to implement the default primary in the constructor of the Generator class. If we implement it into GeneratePrimaries, it will override any macro we want to insert. The priority of execution is: GeneratePrimaries, then the eventual macro, then the constructor. So the macro overrides the constructor, and GeneratePrimaries overrides the macro
   */
  //define particle we want to create
  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName = "mu-";
  /*here, we have written the name geantino, which is just a placeholder for a particle name. If we write any particle name, it will work and produce that particle. For now, geantino will correspond to the Cobalt60*/
  G4ParticleDefinition *particle = particleTable->FindParticle(particleName);

  //define momentum and position of particle. Now it is a muon up-down
  G4ThreeVector position(0.*m, 0.*m, 50*cm); 
  G4ThreeVector momentum(0., 0., -1.);

  //and just set them in the gun
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->GetCurrentSource()->GetEneDist()->SetMonoEnergy(3*GeV);
  fParticleGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);
  fParticleGun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(momentum); 
}

MyPrimaryGenerator::~MyPrimaryGenerator() {
  delete fParticleGun;
}

//everything is an object here... even the event itself
void MyPrimaryGenerator::GeneratePrimaries(G4Event *Event) {
  //get definition of the particle and implement the Cobalt60
  G4ParticleDefinition *particle = fParticleGun->GetParticleDefinition();
  if(particle == G4Geantino::Geantino()) //only if we used the placeholder
    {
      G4int Z = 27;
      G4int A = 60;

      G4double charge = 0.*eplus;
      G4double energy = 0.*keV;

      G4ParticleDefinition *ion = G4IonTable::GetIonTable()->GetIon(Z, A, energy);
      fParticleGun->SetParticleDefinition(ion);
      fParticleGun->SetParticleCharge(charge);
    }
  
  //now simply implement the event
  fParticleGun->GeneratePrimaryVertex(Event);
}
