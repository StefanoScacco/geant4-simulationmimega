#include "Detector.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name){
  quEff = new G4PhysicsFreeVector();
  
  //need to read data of quantum efficiency onto the file we gave
  std::ifstream datafile;
  datafile.open("eff.dat");

  while(1)
    {
      G4double wlen, queff;
      datafile >> wlen >> queff;

      if(datafile.eof())
	break;

      //G4cout << wlen << " " << queff << G4endl;

      //insert what we read into file in the free vector
      quEff->InsertValues(wlen, queff/100);
    }
  datafile.close(); //close the file
}

MySensitiveDetector::~MySensitiveDetector(){
}

G4bool MySensitiveDetector::ProcessHits(G4Step *Step, G4TouchableHistory *R0hist){
    //add here how detector should return information about photons
  G4Track *track = Step->GetTrack(); //extracts information about track of particle
  //track->SetTrackStatus(fStopAndKill);
  
  //select extremal points of acquiring information
  G4StepPoint *preStepPoint = Step->GetPreStepPoint(); //photon enters detector
  G4StepPoint *postStepPoint = Step->GetPostStepPoint(); //photons exits detector
  /* for charged particles and magnetic field we might want to track every single point in the detector, not necessarily entrance and exit point only like for photon, which is chargeless */

  G4ThreeVector posParticle = preStepPoint->GetPosition(); //acquires position at entrance in the detector
  G4ThreeVector momParticle = preStepPoint->GetMomentum(); //acquires momentum at entrance in the detector

  G4double wlen_adim = (1.239841939*eV/momParticle.mag())*1E+03; //wavelength adimensional
  G4double wlen = (1.239841939*eV/momParticle.mag())*1E+03*nm; //wavelength in nm   //finally, write down position
  //G4cout << "Photon position: " << posPhoton << G4endl;

  G4double mass = preStepPoint->GetMass(); //acquires mass of particle at entrance in the detector. For photons, it should be 0
  G4double charge = preStepPoint->GetCharge(); //acquires charge of particle at entrance in the detector. For photons, it should be 0
  G4double energy = preStepPoint->GetTotalEnergy(); //acquires energy of particle at entrance of the detector
  
  /*Global time is distance in time from creation of event in general, while Local time is the time from creation of specific particle. They are different during decaus, for example. Here, they are equal. IF YOU WANT TO IMPLEMENT TIME OF FLIGHT, NEED TO COMMENT LINE track->SetTrackStatus(fStopAndKill);*/
  G4double time = preStepPoint->GetGlobalTime(); //I want to take global time only when entering detector 
    
  G4double eps = 1*MeV; //threshold for photon discrimination
  G4double M_thresh = 100*MeV; //threshold for muon discrimination 
  
  /*in real life, though, we only have access to position of our detector, NOT the position of the photon. Therefore, we should access a "touchable" variable, which in our case should only be WHICH detector has fired the photon*/
  const G4VTouchable *touchable = Step->GetPreStepPoint()->GetTouchable();
  G4int copyNo = touchable->GetCopyNumber(); //we are basically only returning as a "touchable" variable only the UNIQUE index of the detector that signalled
  //G4cout << "Copy number: " << copyNo << G4endl;
    
  //now, translate our copy number that fired into the volume of the detector we are interested in
  G4VPhysicalVolume *physVol = touchable->GetVolume(); //physical volume of the "touchable variable
  G4ThreeVector posDetector = physVol->GetTranslation(); //three vector position translation of that volume
  //G4cout << "Detector position: " << posDetector << G4endl;
  //G4cout << "Photon wavelength: " << wlen << G4endl; //also print the wavelength of the photon detected
  
  //fill the data of the detector to output file. For reference, see Run.cc
  G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID(); //gets event number to save in file
  
  G4AnalysisManager *man = G4AnalysisManager::Instance();
  //order of parameter: number of Ntuple and number of column of the tuple, and only then we write content
  //FIRST NTUPLE: position of photon when it hits detector
  if (mass < eps) {
    man->FillNtupleIColumn(0, 0, evt);
    man->FillNtupleDColumn(0, 1, posParticle[0]);
    man->FillNtupleDColumn(0, 2, posParticle[1]);
    man->FillNtupleDColumn(0, 3, posParticle[2]);
    man->FillNtupleDColumn(0, 4, wlen); //calculated
    man->FillNtupleDColumn(0, 5, mass); //expected 0
    man->FillNtupleDColumn(0, 6, charge); //expected 0
    //man->FillNtupleDColumn(0, 7, time); //for TOF only
    man->AddNtupleRow(0);

    man->FillH1(1, wlen); //real photon wavelength

    //SECOND NTUPLE: position of detector we can measure
    //We need to insert the quantum efficiency to check that the detector actually measures something
    if(G4UniformRand() < quEff->Value(wlen_adim))
      //just generate a random number between 0 and 1. If smaller than quantum efficiency calculated at that specific wlen, the detector measures the photon
      {
	man->FillNtupleIColumn(1, 0, evt);
	man->FillNtupleDColumn(1, 1, posDetector[0]);
	man->FillNtupleDColumn(1, 2, posDetector[1]);
	man->FillNtupleDColumn(1, 3, posDetector[2]);
	man->FillNtupleDColumn(1, 4, wlen);
	man->AddNtupleRow(1);

	man->FillH1(2, wlen); //detected photon wavelength
	//when a photon hits a sensor, class of detector activated and stores position of detector in root file
      }
    
    track->SetTrackStatus(fStopAndKill); //kill here if photon
  }

  else if (mass > M_thresh) {
    man->FillNtupleDColumn(3, 0, time); //store global time hit
    man->FillNtupleIColumn(3, 1, copyNo); //store copy number of muon
    man->FillNtupleDColumn(3, 2, mass); //store mass of signal
    man->FillNtupleDColumn(3, 3, energy); //store energy of signal muon
    man->AddNtupleRow(3);

    man->FillH1(0, energy);
  }

  return 0;
}
