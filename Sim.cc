#include <iostream>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "QGSP_BERT.hh" //standard physics list to simulate hadronic processes

#include "DetectorConstruction.hh"
#include "Physics.hh"
#include "Action.hh"
#include "Run.hh"

int main(int argc, char** argv)
{
  CLHEP::HepRandom::setTheSeed(time(NULL));
  //this implements user's interface
  //G4UIExecutive *ui = new G4UIExecutive(argc, argv);
  G4UIExecutive *ui = 0; //if we have a macro, we don't want an executive here
  
  //run manager is the heart of the simulation: It does everything
  G4RunManager *runManager = new G4RunManager();

  //includes our new classes
  runManager->SetUserInitialization(new MyDetectorConstruction()); 
  runManager->SetUserInitialization(new MyPhysicsList());
  runManager->SetUserInitialization(new MyActionInitialization());

  //new physics process to simulate cosmic shower
  //  G4VModularPhysicsList* physics = new QGSP_BERT();
  //physics->RegisterPhysics(new G4DecayPhysics());
  //runManager->SetUserInitialization(physics);

  runManager->Initialize(); //must initialize to see something

  //if no macro is given, it initializes the manager
  if (argc == 1)
    {
      ui = new G4UIExecutive(argc, argv);
    }
  
  //visualization manager
  G4VisManager *visManager = new G4VisExecutive();
  visManager->Initialize();

  //class for the command manipulation and the user interface
  G4UImanager *UImanager = G4UImanager::GetUIpointer();

  //if the macro is not given, it will simply execute the vis.mac macro
  if (ui)
    {
      //this tells manager to execute macro file vis.mac for visualizations
      UImanager->ApplyCommand("/control/execute vis.mac");
 
      //start the session
      ui->SessionStart();
    }

  /*if there is a macro to execute, we simply tell him to execute the macro name we insert as a user parameter when executing. This is usually better, as we do not need to call the visualizer each time, and the simulation is, hence, faster
   */
  else
    {
      ui = new G4UIExecutive(argc, argv);
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UImanager->ApplyCommand(command+fileName);
      ui->SessionStart();
    }
  
  return 0;
}
