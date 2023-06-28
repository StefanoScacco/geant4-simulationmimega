#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh" //this we created ourselves
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh" //useful to change geometry or other parameters on the fly in visualization mode
#include "G4Types.hh"

#include "Detector.hh"

//create a class derived from Geant4 class for construction of detectors
class MyDetectorConstruction : public G4VUserDetectorConstruction {
public:
  MyDetectorConstruction(); //constructor
  ~MyDetectorConstruction();

  G4LogicalVolume *GetScoringVolume() const { return fScoringVolume; } //this is here to make sure that the energy deposition we have is ONLY calculated over our scintillator and not the rest of the environment
  
  virtual G4VPhysicalVolume *Construct(); //implement later

private:
  //add function predefined in geant4 that construct sensitive detectors and field
  virtual void ConstructSDandField();

  G4bool isCherenkov, isScintillator, isTOF, isAtmosphere;
  G4bool isScintEfficiencyP2, isScintEfficiencyP3, isScintEfficiencyG1N, isScintEfficiencyG1B;
  G4bool isScintDistance;
  G4bool isScintLead;
  //decides which geometry should be loaded: Cherenkov detector, scintillator, time of flight, atmosphere, Scintillator efficiency or distance setup, and if we need lead in the configuration, to stop particles.
  
  G4double xWorld, yWorld, zWorld;
  G4int nCols, nRows; //variables that we want to access

  /*all the volumes should be declared here, and the reason is that if we declare new objects in the cc file, then when we add new volumes on the file we would have memory problems because of reinitialization of those volumes*/
  G4Box *solidWorld, *solidRadiator, *solidDetector, *solidScintillator, *solidAtmosphere;
  G4LogicalVolume *logicWorld, *logicRadiator, *logicDetector, *logicScintillator, *logicAtmosphere[10]; //atmosphere is layered: density changes
  G4VPhysicalVolume *physWorld, *physRadiator, *physDetector, *physScintillator, *physAtmosphere[10]; //atmosphere is layered: density changes
  G4LogicalVolume *fScoringVolume; //introduce the scoring volume, where we can calculate energy deposition

  G4OpticalSurface  *mirrorSurface; //for scintillation case: mirror coating

  //here, we introduce volumes for our scintillators: BN, BN, P2, P3, which are the 4 that we have
  G4Box *solidG1N, *solidG1B, *solidP2, *solidP3;
  G4LogicalVolume *logicG1N, *logicG1B, *logicP2, *logicP3;
  G4VPhysicalVolume *physG1N, *physG1B, *physP2, *physP3;

  //and eventually the lead we use to stop them
  G4Box *solidLead;
  G4LogicalVolume *logicLead;
  G4VPhysicalVolume *physLead;

  //same for materials used
  G4Material *SiO2, *H2O, *Aerogel, *worldMat, *worldVacuum, *NaI, *Lead, *Air[10]; //layers of air
  G4Element *C, *N, *O, *Na, *I, *Pb;

  //define a distance in the ScintDistance setup
  G4double dScint;

  //Messenger to toggle with values in real time
  G4GenericMessenger *fMessenger; //generic messengers to communicate new commands
  G4GenericMessenger *pMessenger;
  
  void DefineMaterials(); //this function defines the aerogel directly so not as to reinitialize it in the main every time we execute

  //we will now implement several different geometries
  void ConstructCherenkov(); 
  void ConstructScintillator();
  void ConstructTOF();
  void ConstructAtmosphere();
  void ConstructScintEfficiencyP2();
  void ConstructScintEfficiencyP3();
  void ConstructScintEfficiencyG1N();
  void ConstructScintEfficiencyG1B();
  void ConstructScintDistance();
};
  
#endif
