#include "Physics.hh"

MyPhysicsList::MyPhysicsList()
{
  RegisterPhysics (new G4EmStandardPhysics());
  RegisterPhysics (new G4OpticalPhysics());
  
  /* this implements correctly the simulated physics we need. We need scintillation light, so electromagnetic interaction and a special class for optical photons. We do not want to include more physics than necessary, because calculations become much longer. */

  /*to handle particle decays, and ions/heavy atoms decays in particular
   */
  RegisterPhysics (new G4DecayPhysics());
  RegisterPhysics (new G4RadioactiveDecayPhysics());
}

MyPhysicsList::~MyPhysicsList()
{
}
