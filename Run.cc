#include "Run.hh"

MyRunAction::MyRunAction(){ //we are trying to make it work by copying stuff from examples/basics/B5/RunAction.cc
  G4AnalysisManager *man = G4AnalysisManager::Instance();
  man->SetVerboseLevel(2); //prints information about output file
  
  //what kind of information we want to store
  //all names of variables MUST NOT BE SPACED
  //PHOTONS
  man->CreateNtuple("Photons", "Photons");
  //IColumn is for integers, DColumn is for doubles
  man->CreateNtupleIColumn("fEvent"); //column of data
  man->CreateNtupleDColumn("fX"); //position X of photon
  man->CreateNtupleDColumn("fY"); //position Y of photon
  man->CreateNtupleDColumn("fZ"); //position Z of photon
  man->CreateNtupleDColumn("fWlen"); //wavelength of actual photon
  man->CreateNtupleDColumn("fMass"); //mass of the registered photon
  man->CreateNtupleDColumn("fCharge"); //charge of the registered photon
  // man->CreateNtupleDColumn("fT"); //this is for TOF
  man->FinishNtuple(0); //end stuff
  
  // here we store the information we can actually measure
  man->CreateNtuple("Hits", "Hits");
  //IColumn is for integers, DColumn is for doubles
  man->CreateNtupleIColumn("fEvent"); //column of data
  man->CreateNtupleDColumn("fX"); //position X of detector
  man->CreateNtupleDColumn("fY"); //position Y of detector
  man->CreateNtupleDColumn("fZ"); //position Z of detector
  man->CreateNtupleDColumn("wavelength"); //wavelength of photon detected
  man->FinishNtuple(1); //end stuff

  //scoring purposes
  man->CreateNtuple("Scoring", "Scoring");
  //we use D for doubles: we want to store energy deposition
  man->CreateNtupleDColumn("fEdep");
  man->FinishNtuple(2);

  //MUONS
  man->CreateNtuple("Muons hit", "Muons hit");
  man->CreateNtupleDColumn("hitTime"); //need time of flight to implement coincidence detection
  man->CreateNtupleIColumn("copyNo"); //returns copy number of fired detector
  man->CreateNtupleDColumn("muonMass"); //returns mass of muon
  man->CreateNtupleDColumn("muonEnergy"); //returns energy of muon
  //from here, we check coincidences
  man->FinishNtuple(3);
  
  //----------------------------------------------------------------------------------------------------
  //now we implement coincidences of detectors that have been hit
  man->CreateNtuple("Coincidences with hits", "Coincidences with hits");

  //Zero hits
  man->CreateNtupleIColumn("HitsA_null"); //no coincidences whatsoever

  //One hit
  man->CreateNtupleIColumn("HitsB_G1N"); //coincidences of G1N
  man->CreateNtupleIColumn("HitsC_G1B"); //coincidences of G1B
  man->CreateNtupleIColumn("HitsD_P2"); //coincidences of P2
  man->CreateNtupleIColumn("HitsE_P3"); //coincidences of P3

  //Two hits
  man->CreateNtupleIColumn("HitsF_G1N_G1B"); //coincidences of G1N and G1B 
  man->CreateNtupleIColumn("HitsG_G1N_P2"); //coincidences of G1N and P2
  man->CreateNtupleIColumn("HitsH_G1N_P3"); //coincidences of G1N and P3
  man->CreateNtupleIColumn("HitsI_G1B_P2"); //coincidences of G1B and P2
  man->CreateNtupleIColumn("HitsJ_G1B_P3"); //coincidences of G1B and P3
  man->CreateNtupleIColumn("HitsK_P2_P3"); //coincidences of P2 and P3

  //Three hits
  man->CreateNtupleIColumn("HitsL_G1N_G1B_P2"); //coincidences of G1N, G1B and P2
  man->CreateNtupleIColumn("HitsM_G1N_G1B_P3"); //coincidences of G1N, G1B and P3
  man->CreateNtupleIColumn("HitsN_G1N_P2_P3"); //coincidences of G1N, P2 and P3
  man->CreateNtupleIColumn("HitsO_G1B_P2_P3"); //coincidences of G1B, P2 and P3

  //Two seen one not even hit (efficiency correction)
  man->CreateNtupleIColumn("SeenP_G1N_G1B_hitP2"); //coincidences of G1N, G1B hit by P2
  man->CreateNtupleIColumn("SeenQ_G1N_G1B_hitP3"); //coincidences of G1N, G1B hit by P3
  man->CreateNtupleIColumn("SeenR_hitG1N_P2_P3"); //coincidences of P2 and P3 hit by G1N
  man->CreateNtupleIColumn("SeenS_hitG1B_P2_P3"); //coincidences of P2 and P3 hit by G1B

  //Four hits
  man->CreateNtupleIColumn("HitsP_G1N_G1B_P2_P3"); //coincidences of G1N, G1B, P2 and P3
  man->FinishNtuple(4);
  

  //----------------------------------------------------------------------------------------------------
  //now we implement coincidences of muons that have been seen
  man->CreateNtuple("Coincidences with seen", "Coincidences with seen");

  //Zero seen
  man->CreateNtupleIColumn("SeenA_null"); //no coincidences whatsoever

  //One seen
  man->CreateNtupleIColumn("SeenB_G1N"); //coincidences of G1N
  man->CreateNtupleIColumn("SeenC_G1B"); //coincidences of G1B
  man->CreateNtupleIColumn("SeenD_P2"); //coincidences of P2
  man->CreateNtupleIColumn("SeenE_P3"); //coincidences of P3

  //Two seen
  man->CreateNtupleIColumn("SeenF_G1N_G1B"); //coincidences of G1N and G1B
  man->CreateNtupleIColumn("SeenG_G1N_P2"); //coincidences of G1N and P2
  man->CreateNtupleIColumn("SeenH_G1N_P3"); //coincidences of G1N and P3
  man->CreateNtupleIColumn("SeenI_G1B_P2"); //coincidences of G1B and P2
  man->CreateNtupleIColumn("SeenJ_G1B_P3"); //coincidences of G1B and P3
  man->CreateNtupleIColumn("SeenK_P2_P3"); //coincidences of P2 and P3

  //Three seen
  man->CreateNtupleIColumn("SeenL_G1N_G1B_P2"); //coincidences of G1N, G1B and P2
  man->CreateNtupleIColumn("SeenM_G1N_G1B_P3"); //coincidences of G1N, G1B and P3
  man->CreateNtupleIColumn("SeenN_G1N_P2_P3"); //coincidences of G1N, P2 and P3
  man->CreateNtupleIColumn("SeenO_G1B_P2_P3"); //coincidences of G1B, P2 and P3

  //Two seen one not even hit (efficiency correction)
  man->CreateNtupleIColumn("SeenP_G1N_G1B_hitP2"); //coincidences of G1N, G1B hit by P2
  man->CreateNtupleIColumn("SeenQ_G1N_G1B_hitP3"); //coincidences of G1N, G1B hit by P3
  man->CreateNtupleIColumn("SeenR_hitG1N_P2_P3"); //coincidences of P2 and P3 hit by G1N
  man->CreateNtupleIColumn("SeenS_hitG1B_P2_P3"); //coincidences of P2 and P3 hit by G1B
  
  //Four seen
  man->CreateNtupleIColumn("SeenT_G1N_G1B_P2_P3"); //coincidences of G1N, G1B, P2 and P3
  man->FinishNtuple(5);

  //try to implement histograms as well
  man->CreateH1("Emuon", "Muon energy", 100, 0.7*GeV, 15.*GeV, "GeV");
  man->CreateH1("Wavelength", "Real scintillation photon wavelength", 100, 180*nm, 920*nm, "nm");
  man->CreateH1("Wavelengthseen", "Detected scintillation photon wavelength", 100, 180*nm, 920*nm, "nm");  
  man->CreateH1("TOF", "Time of flight of muons. 2 scintillators at 60 cm distance", 100, 0*ns, 5*ns, "ns");
}

MyRunAction::~MyRunAction(){
  delete G4AnalysisManager::Instance();
}

void MyRunAction::BeginOfRunAction(const G4Run* Run){
  //whenever we create events, we should create a new file
  G4AnalysisManager *man = G4AnalysisManager::Instance(); //immediate function: no need for new

  //gets number of the run to execute for run.mac, and prints it on screen during simulation
  G4int runID = Run->GetRunID(); //this is instance of run as input (header G4Run.hh)
  std::stringstream strRunID;
  strRunID << runID;
  
  man->Reset();
  man->OpenFile("output"+strRunID.str()+".root"); //update root file name with run number
}

void MyRunAction::EndOfRunAction(const G4Run*){
  //same functions but for closing the file
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  man->Write();
  man->CloseFile(false);
  G4cout << "End of run action" << G4endl;
}
