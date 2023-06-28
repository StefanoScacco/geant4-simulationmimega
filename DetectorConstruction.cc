#include "DetectorConstruction.hh"

/*##############################################################################################################################*/

MyDetectorConstruction::MyDetectorConstruction()
{
  //all the commands we add on the fly need a folder where added, help text to identify the messenger
  fMessenger = new G4GenericMessenger(this, "/detector/", "Detector Construction");
  pMessenger = new G4GenericMessenger(this, "/distanceScint/", "Distance between Scintillators");
  
  //define new command, for number of columns and rows of data. Name, variable, and help text for user when visualizing simulation
  fMessenger->DeclareProperty("Cherenkov", isCherenkov, "Toggle Cherenkov setup");
  fMessenger->DeclareProperty("Scintillator", isScintillator, "Toggle Scintillator setup");
  fMessenger->DeclareProperty("Time of Flight", isTOF, "Toggle TOF setup");
  fMessenger->DeclareProperty("Atmosphere", isAtmosphere, "Toggle Atmosphere setup");
  fMessenger->DeclareProperty("Scintillator Efficiency P2", isScintEfficiencyP2, "Toggle Scintillator efficiency P2 setup");
  fMessenger->DeclareProperty("Scintillator Efficiency P3", isScintEfficiencyP3, "Toggle Scintillator efficiency P3 setup");
  fMessenger->DeclareProperty("Scintillator Efficiency G1N", isScintEfficiencyG1N, "Toggle Scintillator efficiency G1N setup");
  fMessenger->DeclareProperty("Scintillator Efficiency G1B", isScintEfficiencyG1B, "Toggle Scintillator efficiency G1B setup");
  fMessenger->DeclareProperty("Scintillator Distance", isScintDistance, "Toggle Scintillator distance setup");
  
  //this is the messenger for our Cherenkov case
  //fMessenger->DeclareProperty("nCols", nCols, "Number of columns");
  //fMessenger->DeclareProperty("nRows", nRows, "Number of rows");
  nCols = 10;
  nRows = 10;

  pMessenger->DeclareProperty("distance", dScint, "Change Scintillator distance [mm]");
  
  //initial values for pmessenger
  dScint = 40*cm;

  //initialize the new materials that we need
  DefineMaterials();

  //size of half the world volume
  xWorld = 0.5*m;
  yWorld = 0.5*m;
  zWorld = 0.5*m;

  //define which geometry we should load here
  isCherenkov = false;
  isScintillator = false; 
  isTOF = false;
  isAtmosphere = false;
  
  isScintEfficiencyP2 = true;
  isScintEfficiencyP3 = false;
  isScintEfficiencyG1N = false;
  isScintEfficiencyG1B = false;
  isScintDistance = false;
  isScintLead = false;
}

/*##############################################################################################################################*/

MyDetectorConstruction::~MyDetectorConstruction()
{
}

/*##############################################################################################################################*/

void MyDetectorConstruction::DefineMaterials()
{
  G4NistManager *nist = G4NistManager::Instance(); //get immediately all materials available for predefined material

  worldMat = nist->FindOrBuildMaterial("G4_AIR"); //class of material: find it or initialize Air
  worldVacuum = nist->FindOrBuildMaterial("G4_Galactic"); //vacuum
   
  //create new material out of void. Parameters: name, density, number of components
  SiO2 = new G4Material("SiO2", 2.201*g/cm3, 2);

  //then, create the material adding the elements in their molar ratio
  SiO2->AddElement(nist->FindOrBuildElement("Si"), 1);
  SiO2->AddElement(nist->FindOrBuildElement("O"), 2);

  //create water
  H2O = new G4Material("H2O", 1.000*g/cm3, 2);
  H2O->AddElement(nist->FindOrBuildElement("H"), 2);
  H2O->AddElement(nist->FindOrBuildElement("O"), 1);

  //create carbon: use the function element
  C = nist->FindOrBuildElement("C");

  //finally: create our aerogel: it has 3 components
  Aerogel = new G4Material("Aerogel", 0.200*g/cm3, 3);
  //we create che mixture in percentage to aerogel
  Aerogel->AddMaterial(SiO2, 62.5*perCent);
  Aerogel->AddMaterial(H2O, 37.4*perCent);
  Aerogel->AddElement(C, 0.1*perCent);

  //time to introduce the refractive index of aerogel.
  //first, energy range of photons with conversion
  G4double energy[2] = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
  G4double rindexAerogel[2] = {1.1, 1.1}; //value at 200nm and 900nm but we imagine it constant in that interval of energy of photons
  G4double rindexWorld[2] = {1.0, 1.0}; //same for the air
  G4double rindexNaI[2] = {1.78, 1.78}; //same for NaI 

  //for the scintillation property
  G4double fraction[2] = {1.0, 1.0}; //same photons in red and blue... it's a simple model

  //for mirror coating
  G4double reflectivity[2] = {1.0, 1.0}; //ideal reflectivity, not depending on energy
  
  G4MaterialPropertiesTable *mptAerogel = new G4MaterialPropertiesTable();
  //RINDEX is string Geant4 associates with refractive index
  //we define a 1:1 correspondence between energies value and refractive index, so we write array and rindex values, and we tell there are 2 values to unpack
  mptAerogel->AddProperty("RINDEX", energy, rindexAerogel, 2);
  Aerogel->SetMaterialPropertiesTable(mptAerogel); //finally, insert the table with our pointer

  //same thing with world
  G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
  mptWorld->AddProperty("RINDEX", energy, rindexWorld, 2);
  //we can add the property to the material of the world
  worldMat->SetMaterialPropertiesTable(mptWorld);

  //implement NaI material
  Na = nist->FindOrBuildElement("Na");
  I = nist->FindOrBuildElement("I");
  NaI = new G4Material("NaI", 3.67*g/cm3, 2);
  NaI->AddElement(Na, 1);
  NaI->AddElement(I, 1);

  //implement now the NaI scintillation property
  G4MaterialPropertiesTable *mptNaI = new G4MaterialPropertiesTable();
  mptNaI->AddProperty("RINDEX", energy, rindexNaI, 2);
  mptNaI->AddProperty("SCINTILLATIONCOMPONENT1", energy, fraction, 2); //number of photons created in scintillation per energy scale
  mptNaI->AddConstProperty("SCINTILLATIONYIELD", 38./MeV); //LY for energy, constant. Original value was 38./keV but it was too slow
  mptNaI->AddConstProperty("RESOLUTIONSCALE", 1.0); //I have no idea 
  mptNaI->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 250*ns); //response time for scintillation light: it's decay time of energy level excited
  mptNaI->AddConstProperty("SCINTILLATIONYIELD1", 1.); //distribution of the photon emitted: fluctuation and eventual sigma parameter
  //add those properties
  NaI->SetMaterialPropertiesTable(mptNaI);


  //implement a reflective coating surface
  mirrorSurface = new G4OpticalSurface("mirrorSurface");
  mirrorSurface->SetType(dielectric_metal); //material type with coating
  mirrorSurface->SetFinish(ground);
  mirrorSurface->SetModel(unified); //standard stuff
  //this coating has to have a reflectivity
  G4MaterialPropertiesTable *mptMirror = new G4MaterialPropertiesTable();
  mptMirror->AddProperty("REFLECTIVITY", energy, reflectivity, 2);
  mirrorSurface->SetMaterialPropertiesTable(mptMirror);


  //define Air now
  G4double density0 = 1.29*kg/m3;
  //contain only N, O: define them
  G4double aN = 14.01*g/mole;
  G4double aO = 16.00*g/mole;
  N = new G4Element("Nitrogen", "N", 7, aN);
  O = new G4Element("Oxygen", "O", 8, aO);

  //implement modeling of Air
  G4double f = 3; //number of dof
  G4double R = 8.3144626181532; //constant of gas
  G4double g0 = 9.81; //gravitational acceleration
  G4double kappa = (f+2)/f; //constant for thermodynamics
  G4double T = 293.15; //Temperature on Earth's surface
  G4double M = (0.3*aO + 0.7*aN)/1000; //molar mass of air if mixture of 70% N and 30% O

  for(G4int i=0; i<10; i++) {
    std::stringstream stri;
    stri << i;
    G4double h = 2*zWorld/10.*i; //height of each layer
    //model the Air using a Wikipedia formula
    G4double density = density0*pow( (1-(kappa)/kappa*M*g0*h/(R*T)), (1/(kappa-1)) );
    Air[i] = new G4Material("G4_AIR "+stri.str(), density, 2); //create air
    Air[i]->AddElement(N, 70*perCent);
    Air[i]->AddElement(O, 30*perCent);
  }

  //implement lead for stopping other rays.
  Pb = nist->FindOrBuildElement("Pb");
  Lead = new G4Material("Lead", 11.4*g/cm3, 1);
  Lead->AddElement(Pb, 1);
}

/*##############################################################################################################################*/

void MyDetectorConstruction::ConstructCherenkov() {
  //now, create our detector volume for experiment. Radiator very thin for Cherenkov detector
  solidRadiator = new G4Box("solidRadiator", 0.4*m, 0.4*m, 0.01*m);

  //now, logical volume in aerogel
  logicRadiator = new G4LogicalVolume(solidRadiator, Aerogel, "logicalRadiator");
  //this will also be the scoring volume in which we are allowed to calculate the energy
  fScoringVolume = logicRadiator;

  //finally, physical volume: careful, mother volume is logic world
  physRadiator = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.25*m), logicRadiator, "physRadiator", logicWorld, false, 0, true);

  //ADD DETECTORS FOR PHOTONS: parametrize for messengers
  solidDetector = new G4Box("solidDetector", xWorld/nRows, yWorld/nCols, 0.01*m);
  logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");

  //cover every part of the mother volume surface behind aerogel with detect\
ors
  //to do it generically for messengers, we parametrize it
  for(G4int i=0; i<nRows; i++){
    for(G4int j=0; j<nCols; j++){
      //careful about positioning. Also, copy number characterizes every detector (diferent number for each detector
      //also, careful about never crossing border of mother volume (-0.5 everytime)
      physDetector = new G4PVPlacement(0, G4ThreeVector(-xWorld+(i+0.5)*m/nRows, -yWorld+(j+0.5)*m/nCols, 0.49*m), logicDetector, "physDetector", logicWorld, false, j+i*nCols, true);
    }
  }
}

/*##############################################################################################################################*/

void MyDetectorConstruction::ConstructScintillator() {
  //implement our scintillator: inner radius, outer radius, length, starting angle and end angle
  solidScintillator = new G4Box("solidScintillator", 5*cm, 5*cm, 6*cm);

  //implement the logic scintillator: geometrical volume, material, name
  logicScintillator = new G4LogicalVolume(solidScintillator, NaI, "logicScintillator");

  //implement the photon detector next to the scintillator: same dimensions in y, z, just thinner
  solidDetector = new G4Box("solidDetector", 1*cm, 5*cm, 6*cm); 

  //implement the logic detector
  logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");
  
  //need to update our scoring volume in the scintillator setup
  fScoringVolume = logicScintillator;

  //fill the world volume with scintillators around Cobalt60 to decay
  for (G4int i=0; i<6; i++) {
    for (G4int j=0; j<16; j++) {
      //it is a set of scintillators in a cylindrical manner around body
      G4Rotate3D rotZ(j*22.5*deg, G4ThreeVector(0, 0, 1));
      G4Translate3D transXScint(G4ThreeVector(5./tan(22.5/2*deg)*cm + 5.*cm, 0.*cm, -40*cm + i*15*cm));
      G4Transform3D transformScint = (rotZ)*(transXScint);
      
      //implement physical volume: rotation, position, logic volume, name, mother volume, copy number, overlap check. Note that we now position them accoring to the rotation we wrote before: transformScint 
      physScintillator = new G4PVPlacement(transformScint, logicScintillator, "physScintillator", logicWorld, false, 0, true);

      //now, position the detector translating again by 5*cm more
      G4Translate3D transXDet(G4ThreeVector(5./tan(22.5/2*deg)*cm + 6.*cm + 5*cm, 0.*cm, -40*cm + i*15*cm));
      G4Transform3D transformDet = (rotZ)*(transXDet);
      //implement physical detectors
      physDetector = new G4PVPlacement(transformDet, logicDetector,
 "physDetector", logicWorld, false, 0, true);
    }
  }

  //create the coating on the scintillator
  G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin", logicWorld, mirrorSurface);
}

/*##############################################################################################################################*/

void MyDetectorConstruction::ConstructTOF() {
  //we construct a simple setup to see TOF of particles
  solidDetector = new G4Box("solidDetector", 1.*m, 1.*m, 0.1*m);
  logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");
  
  physDetector = new G4PVPlacement(0, G4ThreeVector(0.*m, 0.*m, -4.*m), logicDetector, "physDetector", logicWorld, false, 0, true); //copy number 0
  physDetector = new G4PVPlacement(0, G4ThreeVector(0.*m, 0.*m, 3.*m),	logicDetector, "physDetector", logicWorld, false, 1, true); //copy number 1
}

/*##############################################################################################################################*/

void MyDetectorConstruction::ConstructAtmosphere() {
  //here we implement atmospheric construction
  solidAtmosphere = new G4Box("solidAtmosphere", xWorld, yWorld, zWorld/10.);
  //define 1/10 of atmospheric layer

  //implement material in logic volume 10 times with Air[i]
  for(G4int i=0; i<10; i++) {
    //implement logic volumes
    logicAtmosphere[i] = new G4LogicalVolume(solidAtmosphere, Air[i], "logicAtmosphere");

    //implement physical volumes
    physAtmosphere[i] = new G4PVPlacement(0, G4ThreeVector(0., 0., zWorld/10.*2*i - zWorld + zWorld/10.), logicAtmosphere[i], "physAtmosphere", logicWorld, false, i, true); //copy number is i between 0, 9
  }
}

/*##############################################################################################################################*/

void MyDetectorConstruction::ConstructScintEfficiencyP2() {
  //here we implement setup for our scintillators in the lab: dimensions are half of what is needed
  solidG1N = new G4Box("solidG1N", 10*cm, 5.*cm, 0.5*cm);
  solidG1B = new G4Box("solidG1B", 10*cm, 5.*cm, 0.5*cm);
  solidP2 = new G4Box("solidP2", 7.5*cm, 5.*cm, 0.5*cm);
  solidP3 = new G4Box("solidP3", 7.5*cm, 5.*cm, 0.5*cm);

  //implement logic volumes
  logicG1N = new G4LogicalVolume(solidG1N, NaI, "logicG1N");
  logicG1B = new G4LogicalVolume(solidG1B, NaI, "logicG1B");
  logicP2 = new G4LogicalVolume(solidP2, NaI, "logicP2");
  logicP3 = new G4LogicalVolume(solidP3, NaI, "logicP3");

  //setup rotations and translation
  G4Rotate3D rotZ(90*deg, G4ThreeVector(0, 0, 1));
  
  G4Translate3D transXYScint(G4ThreeVector(-5*cm, -2.5*cm, 21.*cm)); //for up scintillators
  G4Transform3D transformScint = (rotZ)*(transXYScint);

  G4Translate3D transXYDet1(G4ThreeVector(2.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet1 = (rotZ)*(transXYDet1);

  G4Translate3D transXYDet2(G4ThreeVector(-2.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet2 =	(rotZ)*(transXYDet2);

  G4Translate3D transXYDet3(G4ThreeVector(-7.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet3 =	(rotZ)*(transXYDet3);

  G4Translate3D transXYDet4(G4ThreeVector(-12.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet4 =	(rotZ)*(transXYDet4);
  
  //implement physical volumes
  physG1N = new G4PVPlacement(transformScint, logicG1N, "physG1N", logicWorld, false, 0, true);
  physG1B = new G4PVPlacement(0, G4ThreeVector(-2.5*cm, 0., -2.*cm), logicG1B, "physG1B", logicWorld, false, 0, true);
  physP2 = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicP2, "physP2", logicWorld, false, 0, true);

  //create detectors
  //implement the photon detector next to the scintillator: same dimensions in y, z, just thinner
  solidDetector = new G4Box("solidDetector", 2.5*cm, 5.*cm, 0.5*cm);
  logicDetector = new G4LogicalVolume(solidDetector, worldVacuum, "logicDetector");
    
  /* It is fundamental to add the detectors with the following copy numbers:
   G1N: every detector related to it has copy number 2
   G1B: every detector related to it has copy number 3
   P2: every detector related to it has copy number 5
   P3: every detector related to it has copy number 7*/
  physDetector = new G4PVPlacement(transformDet1, logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  physDetector = new G4PVPlacement(transformDet2, logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  physDetector = new G4PVPlacement(transformDet3, logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  physDetector = new G4PVPlacement(transformDet4, logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  
  physDetector = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorP2", logicWorld, false, 5, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(5*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorP2", logicWorld, false, 5, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-5*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorP2", logicWorld, false, 5, true);

  physDetector = new G4PVPlacement(0, G4ThreeVector(5.*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-5.*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-10.*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  
  //create the coating on the scintillator
  G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin", logicWorld, mirrorSurface);

  if (isScintLead)
    {
      solidLead = new G4Box("solidLead", 10*cm, 10*cm, 5.*cm);
      logicLead = new G4LogicalVolume(solidLead, Lead, "logicLead");
      physLead = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, 10.*cm), logicLead, "physLead", logicWorld, false, 1, true);
      G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skinLead", logicLead, mirrorSurface);
    }

  //implement scoring volume
  fScoringVolume = logicDetector;
}

/*##############################################################################################################################*/

void MyDetectorConstruction::ConstructScintEfficiencyP3() {
  //here we implement setup for our scintillators in the lab: dimensions are half of what is needed
  solidG1N = new G4Box("solidG1N", 10*cm, 5.*cm, 0.5*cm);
  solidG1B = new G4Box("solidG1B", 10*cm, 5.*cm, 0.5*cm);
  solidP2 = new G4Box("solidP2", 7.5*cm, 5.*cm, 0.5*cm);
  solidP3 = new G4Box("solidP3", 7.5*cm, 5.*cm, 0.5*cm);

  //implement logic volumes
  logicG1N = new G4LogicalVolume(solidG1N, NaI, "logicG1N");
  logicG1B = new G4LogicalVolume(solidG1B, NaI, "logicG1B");
  logicP2 = new G4LogicalVolume(solidP2, NaI, "logicP2");
  logicP3 = new G4LogicalVolume(solidP3, NaI, "logicP3");

  //setup rotations and translation
  G4Rotate3D rotZ(90*deg, G4ThreeVector(0, 0, 1));

  G4Translate3D transXYScint(G4ThreeVector(-5*cm, -2.5*cm, 21.*cm)); //for up scintillators
  G4Transform3D transformScint = (rotZ)*(transXYScint);

  G4Translate3D transXYDet1(G4ThreeVector(2.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet1 = (rotZ)*(transXYDet1);

  G4Translate3D transXYDet2(G4ThreeVector(-2.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet2 = (rotZ)*(transXYDet2);

  G4Translate3D transXYDet3(G4ThreeVector(-7.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet3 = (rotZ)*(transXYDet3);

  G4Translate3D transXYDet4(G4ThreeVector(-12.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet4 = (rotZ)*(transXYDet4);

  //implement physical volumes
  physG1N = new G4PVPlacement(transformScint, logicG1N, "physG1N", logicWorld, false, 0, true);
  physG1B = new G4PVPlacement(0, G4ThreeVector(-2.5*cm, 0., -2.*cm), logicG1B, "physG1B", logicWorld, false, 0, true);
  physP3 = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicP3, "physP3", logicWorld, false, 0, true);
  
  //create detectors
  //implement the photon detector next to the scintillator: same dimensions in y, z, just thinner
  solidDetector = new G4Box("solidDetector", 2.5*cm, 5.*cm, 0.5*cm);
  logicDetector = new G4LogicalVolume(solidDetector, worldVacuum, "logicDetector");

  /* It is fundamental to add the detectors with the following copy numbers:
   G1N: every detector related to it has copy number 2
   G1B: every detector related to it has copy number 3
   P2: every detector related to it has copy number 5
   P3: every detector related to it has copy number 7*/
  physDetector = new G4PVPlacement(transformDet1, logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  physDetector = new G4PVPlacement(transformDet2, logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  physDetector = new G4PVPlacement(transformDet3, logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  physDetector = new G4PVPlacement(transformDet4, logicDetector, "physDetectorG1N", logicWorld, false, 2, true);

  physDetector = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorP3", logicWorld, false, 7, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(5*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorP3", logicWorld, false, 7, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-5*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorP3", logicWorld, false, 7, true);

  physDetector = new G4PVPlacement(0, G4ThreeVector(5.*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-5.*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-10.*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);

  //create the coating on the scintillator
  G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin", logicWorld, mirrorSurface);

  if (isScintLead)
    {
      solidLead = new G4Box("solidLead", 10*cm, 10*cm, 2.5*cm);
      logicLead = new G4LogicalVolume(solidLead, Lead, "logicLead");
      physLead = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, 10.*cm), logicLead, "physLead", logicWorld, false, 1, true);
      G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skinLead", logicLead, mirrorSurface);
    }
  
  //implement scoring volume
  fScoringVolume = logicDetector;
}

/*##############################################################################################################################*/

void MyDetectorConstruction::ConstructScintEfficiencyG1N() {
  //here we implement setup for our scintillators in the lab: dimensions are half of what is needed
  solidG1N = new G4Box("solidG1N", 10*cm, 5.*cm, 0.5*cm);
  solidG1B = new G4Box("solidG1B", 10*cm, 5.*cm, 0.5*cm);
  solidP2 = new G4Box("solidP2", 7.5*cm, 5.*cm, 0.5*cm);
  solidP3 = new G4Box("solidP3", 7.5*cm, 5.*cm, 0.5*cm);

  //implement logic volumes
  logicG1N = new G4LogicalVolume(solidG1N, NaI, "logicG1N");
  logicG1B = new G4LogicalVolume(solidG1B, NaI, "logicG1B");
  logicP2 = new G4LogicalVolume(solidP2, NaI, "logicP2");
  logicP3 = new G4LogicalVolume(solidP3, NaI, "logicP3");

  //setup rotations and translation
  G4Rotate3D rotZ(90*deg, G4ThreeVector(0, 0, 1));

  G4Translate3D transXYScint(G4ThreeVector(2.5*cm, -2.5*cm, 21.*cm)); //for up scintillators
  G4Transform3D transformScint = (rotZ)*(transXYScint);

  G4Translate3D transXYDet1(G4ThreeVector(2.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet1 = (rotZ)*(transXYDet1);

  G4Translate3D transXYDet2(G4ThreeVector(-2.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet2 = (rotZ)*(transXYDet2);

  G4Translate3D transXYDet3(G4ThreeVector(7.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet3 = (rotZ)*(transXYDet3);

  //implement physical volumes
  physP3 = new G4PVPlacement(transformScint, logicP3, "physP3", logicWorld, false, 0, true);
  physP2 = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0., -2.*cm), logicP2, "physP2", logicWorld, false, 0, true);
  physG1N = new G4PVPlacement(0, G4ThreeVector(-2.5*cm, 0., 0.), logicG1N, "physG1N", logicWorld, false, 0, true);

  //create detectors
  //implement the photon detector next to the scintillator: same dimensions in y, z, just thinner
  solidDetector = new G4Box("solidDetector", 2.5*cm, 5.*cm, 0.5*cm);
  logicDetector = new G4LogicalVolume(solidDetector, worldVacuum, "logicDetector");

  /* It is fundamental to add the detectors with the following copy numbers:
   G1N: every detector related to it has copy number 2
   G1B: every detector related to it has copy number 3
   P2: every detector related to it has copy number 5
   P3: every detector related to it has copy number 7*/
  physDetector = new G4PVPlacement(transformDet1, logicDetector, "physDetectorP3", logicWorld, false, 7, true);
  physDetector = new G4PVPlacement(transformDet2, logicDetector, "physDetectorP3", logicWorld, false, 7, true);
  physDetector = new G4PVPlacement(transformDet3, logicDetector, "physDetectorP3", logicWorld, false, 7, true);

  physDetector = new G4PVPlacement(0, G4ThreeVector(0*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(5*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-5*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-10*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  
  physDetector = new G4PVPlacement(0, G4ThreeVector(5*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorP2", logicWorld, false, 5, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(0*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorP2", logicWorld, false, 5, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-5*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorP2", logicWorld, false, 5, true);

  //create the coating on the scintillator
  G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin", logicWorld, mirrorSurface);

  if (isScintLead)
    {
      solidLead = new G4Box("solidLead", 10*cm, 10*cm, 2.5*cm);
      logicLead = new G4LogicalVolume(solidLead, Lead, "logicLead");
      physLead = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, 10.*cm), logicLead, "physLead", logicWorld, false, 1, true);
      G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skinLead", logicLead, mirrorSurface);
    }
  
  //implement scoring volume
  fScoringVolume = logicDetector;
}

/*##############################################################################################################################*/

void MyDetectorConstruction::ConstructScintEfficiencyG1B() {
  //here we implement setup for our scintillators in the lab: dimensions are half of what is needed
  solidG1N = new G4Box("solidG1N", 10*cm, 5.*cm, 0.5*cm);
  solidG1B = new G4Box("solidG1B", 10*cm, 5.*cm, 0.5*cm);
  solidP2 = new G4Box("solidP2", 7.5*cm, 5.*cm, 0.5*cm);
  solidP3 = new G4Box("solidP3", 7.5*cm, 5.*cm, 0.5*cm);

  //implement logic volumes
  logicG1N = new G4LogicalVolume(solidG1N, NaI, "logicG1N");
  logicG1B = new G4LogicalVolume(solidG1B, NaI, "logicG1B");
  logicP2 = new G4LogicalVolume(solidP2, NaI, "logicP2");
  logicP3 = new G4LogicalVolume(solidP3, NaI, "logicP3");

  //setup rotations and translation
  G4Rotate3D rotZ(90*deg, G4ThreeVector(0, 0, 1));

  G4Translate3D transXYScint(G4ThreeVector(2.5*cm, -2.5*cm, 21.*cm)); //for up scintillators
  G4Transform3D transformScint = (rotZ)*(transXYScint);

  G4Translate3D transXYDet1(G4ThreeVector(2.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet1 = (rotZ)*(transXYDet1);

  G4Translate3D transXYDet2(G4ThreeVector(-2.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet2 = (rotZ)*(transXYDet2);

  G4Translate3D transXYDet3(G4ThreeVector(7.5*cm, -2.5*cm, 20.*cm)); //for detectors below that
  G4Transform3D transformDet3 = (rotZ)*(transXYDet3);

  //implement physical volumes
  physP3 = new G4PVPlacement(transformScint, logicP3, "physP3", logicWorld, false, 0, true);
  physP2 = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0., -2.*cm), logicP2, "physP2", logicWorld, false, 0, true);
  physG1N = new G4PVPlacement(0, G4ThreeVector(-2.5*cm, 0., 0.), logicG1B, "physG1B", logicWorld, false, 0, true);

  //create detectors
  //implement the photon detector next to the scintillator: same dimensions in y, z, just thinner
  solidDetector = new G4Box("solidDetector", 2.5*cm, 5.*cm, 0.5*cm);
  logicDetector = new G4LogicalVolume(solidDetector, worldVacuum, "logicDetector");

  /* It is fundamental to add the detectors with the following copy numbers:
   G1N: every detector related to it has copy number 2
   G1B: every detector related to it has copy number 3
   P2: every detector related to it has copy number 5
   P3: every detector related to it has copy number 7*/
  physDetector = new G4PVPlacement(transformDet1, logicDetector, "physDetectorP3", logicWorld, false, 7, true);
  physDetector = new G4PVPlacement(transformDet2, logicDetector, "physDetectorP3", logicWorld, false, 7, true);
  physDetector = new G4PVPlacement(transformDet3, logicDetector, "physDetectorP3", logicWorld, false, 7, true);

  physDetector = new G4PVPlacement(0, G4ThreeVector(0*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(5*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-5*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-10*cm, 0.*cm, -1.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);

  physDetector = new G4PVPlacement(0, G4ThreeVector(5*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorP2", logicWorld, false, 5, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(0*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorP2", logicWorld, false, 5, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-5*cm, 0.*cm, -3.*cm), logicDetector, "physDetectorP2", logicWorld, false, 5, true);

  //create the coating on the scintillator
  G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin", logicWorld, mirrorSurface);

  if (isScintLead)
    {
      solidLead = new G4Box("solidLead", 10*cm, 10*cm, 2.5*cm);
      logicLead = new G4LogicalVolume(solidLead, Lead, "logicLead");
      physLead = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, 10.*cm), logicLead, "physLead", logicWorld, false, 1, true);
      G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skinLead", logicLead, mirrorSurface);
    }
  
  //implement scoring volume
  fScoringVolume = logicDetector;
}

/*##############################################################################################################################*/

void MyDetectorConstruction::ConstructScintDistance() {
  //here we implement setup for our scintillators in the lab: dimensions are half of what is needed
  solidG1N = new G4Box("solidG1N", 10*cm, 5.*cm, 0.5*cm);
  solidG1B = new G4Box("solidG1B", 10*cm, 5.*cm, 0.5*cm);
  solidP2 = new G4Box("solidP2", 7.5*cm, 5.*cm, 0.5*cm);
  solidP3 = new G4Box("solidP3", 7.5*cm, 5.*cm, 0.5*cm);

  //implement logic volumes
  logicG1N = new G4LogicalVolume(solidG1N, NaI, "logicG1N");
  logicG1B = new G4LogicalVolume(solidG1B, NaI, "logicG1B");
  logicP2 = new G4LogicalVolume(solidP2, NaI, "logicP2");
  logicP3 = new G4LogicalVolume(solidP3, NaI, "logicP3");

  //implement physical volumes
  physG1N = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -dScint/2), logicG1N, "physG1N", logicWorld, false, 0, true);
  physG1B = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, dScint/2), logicG1B, "physG1B", logicWorld, false, 0, true);

  //create detectors
  //implement the photon detector next to the scintillator: same dimensions in y, z, just thinner
  solidDetector = new G4Box("solidDetector", 2.5*cm, 5.*cm, 0.5*cm);
  logicDetector = new G4LogicalVolume(solidDetector, worldVacuum, "logicDetector");

  /* It is fundamental to add the detectors with the following copy numbers:
   G1N: every detector related to it has copy number 2
   G1B: every detector related to it has copy number 3
   P2: every detector related to it has copy number 5
   P3: every detector related to it has copy number 7*/
  physDetector = new G4PVPlacement(0, G4ThreeVector(-7.5*cm, 0.*cm, -dScint/2-1.*cm), logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-2.5*cm, 0.*cm, -dScint/2-1.*cm), logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(2.5*cm, 0.*cm, -dScint/2-1.*cm), logicDetector, "physDetectorG1N", logicWorld, false, 2, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(7.5*cm, 0.*cm, -dScint/2-1.*cm), logicDetector, "physDetectorG1N", logicWorld, false, 2, true);

  physDetector = new G4PVPlacement(0, G4ThreeVector(-7.5*cm, 0.*cm, dScint/2-1.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(-2.5*cm, 0.*cm, dScint/2-1.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(2.5*cm, 0.*cm, dScint/2-1.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);
  physDetector = new G4PVPlacement(0, G4ThreeVector(7.5*cm, 0.*cm, dScint/2-1.*cm), logicDetector, "physDetectorG1B", logicWorld, false, 3, true);

  //create the coating on the scintillator
  G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin", logicWorld, mirrorSurface);

  if (isScintLead)
    {
      solidLead = new G4Box("solidLead", 10*cm, 10*cm, 2.5*cm);
      logicLead = new G4LogicalVolume(solidLead, Lead, "logicLead");
      physLead = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, 10.*cm), logicLead, "physLead", logicWorld, false, 1, true);
      G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skinLead", logicLead, mirrorSurface);
    }
  
  //implement scoring volume
  fScoringVolume = logicDetector;
}

/*##############################################################################################################################*/

G4VPhysicalVolume *MyDetectorConstruction::Construct() //implement here world volume only
{
  //world volume is everything we build everything in. It is the volume that must be returned.
  /*each volume has three parts: world volume (geometry), logical volume (material and properties), and physical world (that interacts with particles.
   If there is overlapping between two volumes, ONE MUST INCORPORATE THE OTHER, and be a mother volume for it*/

  //DEFINE THE VOLUMES OF OUR SIMULATION
  //standard unit is millimeter. To use meters just write it, as we included class G4SystemOfUnits
  solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);//this is world of our project. Note: dimensions are HALF the length!

  logicWorld = new G4LogicalVolume(solidWorld, worldVacuum, "logicWorld"); //need geometrical volume, material, name

  physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);
  //parameters: rotation, displacement, logical volume, name, mother volume (if not mother volume, put 0), boolean operation, copy number, check overlap (always true) 

  //now, conditions of world creation. If we select Cherenkov setup, we create it. Otherwise we create the scintillator one, and so on and so forth... 
  if(isCherenkov)
    ConstructCherenkov();

  if (isScintillator)
    ConstructScintillator();

  if(isTOF)
    ConstructTOF();

  if(isAtmosphere)
    ConstructAtmosphere();

  if(isScintEfficiencyP2)
    ConstructScintEfficiencyP2();

  if(isScintEfficiencyP3)
    ConstructScintEfficiencyP3();

  if(isScintEfficiencyG1N)
    ConstructScintEfficiencyG1N();

  if(isScintEfficiencyG1B)
    ConstructScintEfficiencyG1B();

  if(isScintDistance)
    ConstructScintDistance();
  
  return physWorld;
  
}

/*##############################################################################################################################*/

//implement new G4 function for detectors
void MyDetectorConstruction::ConstructSDandField()
{
  //allocate instance of new class in Detector.hh
  MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
  //sensitive detector loaded only if defined in any setup
  if (logicDetector != NULL)
    logicDetector->SetSensitiveDetector(sensDet);

  if (isScintLead)
    logicLead->SetSensitiveDetector(sensDet);
  
}
