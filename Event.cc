#include "Event.hh"

MyEventAction::MyEventAction(MyRunAction*) {
  fEdep = 0.; //initialize the variable of energy deposition
  fCopyallHit = 1; //initialize the variable of copy all numbers 
  fCopyallSeen = 1; 
  ID = 0;
}

MyEventAction::~MyEventAction() {
}

void MyEventAction::BeginOfEventAction(const G4Event*)
{
  fEdep = 0.; //reset energy value whenever event is restarted
  fCopyallHit = 1; //reset fCopyAllHit and fCopyallSeen
  fCopyallSeen = 1;
}

void MyEventAction::EndOfEventAction(const G4Event*)
{
  //just print the whole energy deposited
  //G4cout << "Energy deposition: " << fEdep << G4endl;
  //G4cout << "Coincidences hit: " << fCopyallHit << G4endl;
    
  //access the root file and fill with energy 
  G4AnalysisManager *man = G4AnalysisManager::Instance();
  man->FillNtupleDColumn(2, 0, fEdep);
  man->AddNtupleRow(2);
  //G4cout << "Deposited energy: " << fEdep << G4endl;

  //create the efficiencies of the detectors
  /*Also, here are the rules for reading files
    First row of Vpot.dat is for G1N. Efficiency value stored in Eff[0]. Copy number is 2
    Second row of Vpot.dat is for G1B. Efficiency value stored in Eff[1]. Copy number is 3
    Third row of Vpot.dat is for P2. Efficiency value stored in Eff[2]. Copy number is 5
    Fourth row of Vpot.dat is for P3. Efficiency value stored in Eff[3]. Copy number is 7
   */
  G4double Vpot;
  std::ifstream datafile;
  datafile.open("Vpot.dat");

  datafile >> Vpot;
  Eff[0] = effG1N(Vpot);
  datafile >> Vpot;
  Eff[1] = effG1B(Vpot);
  datafile >> Vpot;
  Eff[2] = effP2(Vpot);
  datafile >> Vpot;
  Eff[3] = effP3(Vpot);

  datafile.close(); //close the file

  //G4cout << "Eff[0] = " << Eff[0] << G4endl;
  //G4cout << "Eff[1] = " << Eff[1] << G4endl;
  //G4cout << "Eff[2] = " << Eff[2] << G4endl;
  //G4cout << "Eff[3] = " << Eff[3] << G4endl;

  //trick to save the exact number of events
  G4int NRuns = 833000;
  
  //access the root file and fill with the coincidences
  /* Here are the rules for filling up coincidences
     null: fCopyallHit = 1
     G1N: fCopyallHit divisible by 2
     G1B: fCopyallHit divisible by 3
     P2: fCopyallHit divisible by 5
     P3: fCopyallHit divisible by 7
     G1N-G1B: fCopyallHit divisible by 6
     G1N-P2: fCopyallHit divisible by 10
     G1N-P3: fCopyallHit divisible by 14
     G1B-P2: fCopyallHit divisible by 15
     G1B-P3: fCopyallHit divisible by 21
     P2-P3: fCopyallHit divisible by 35
     G1N-G1B-P2: fCopyallHit divisible by 30
     G1N-G1B-P3: fCopyallHit divisible by 42
     G1N-P2-P3: fCopyallHit divisible by 70
     G1B-P2-P3: fCopyallHit divisible by 105
     G1N-G1B-P2-P3: fCopyallHit divisible by 210
   */

  if (fCopyallHit == 1)     man->FillNtupleIColumn(4, 0, NRuns); //no coincidences
  if (fCopyallHit%2 == 0)
    {
      man->FillNtupleIColumn(4, 1, NRuns); //G1N only
      if (G4UniformRand() < Eff[0]) fCopyallSeen*=2; //may be seen or not
    }
  
  if (fCopyallHit%3 == 0)
    {
      man->FillNtupleIColumn(4, 2, NRuns); //G1B only
      if (G4UniformRand() < Eff[1]) fCopyallSeen*=3; //may be seen or not
    }
  
  if (fCopyallHit%5 == 0)
    {
      man->FillNtupleIColumn(4, 3, NRuns); //P2 only
      if (G4UniformRand() < Eff[2]) fCopyallSeen*=5; //may be seen or not
    }

  if (fCopyallHit%7 == 0)
    {
      man->FillNtupleIColumn(4, 4, NRuns); //P3 only
      if (G4UniformRand() < Eff[3]) fCopyallSeen*=7; //may be seen or not
    }
  if (fCopyallHit%6 == 0)   man->FillNtupleIColumn(4, 5, NRuns); //coincidence G1N-G1B
  if (fCopyallHit%10 == 0)  man->FillNtupleIColumn(4, 6, NRuns); //coincidence G1N-P2
  if (fCopyallHit%14 == 0)  man->FillNtupleIColumn(4, 7, NRuns); //coincidence G1N-P3
  if (fCopyallHit%15 == 0)  man->FillNtupleIColumn(4, 8, NRuns); //coincidence G1B-P2
  if (fCopyallHit%21 == 0)  man->FillNtupleIColumn(4, 9, NRuns); //coincidence G1B-P3
  if (fCopyallHit%35 == 0)  man->FillNtupleIColumn(4, 10, NRuns); //coincidence P2-P3
  if (fCopyallHit%30 == 0)  man->FillNtupleIColumn(4, 11, NRuns); //coincidence G1N-G1B-P2
  if (fCopyallHit%42 == 0)  man->FillNtupleIColumn(4, 12, NRuns); //coincidence G1N-G1B-P3
  if (fCopyallHit%70 == 0)  man->FillNtupleIColumn(4, 13, NRuns); //coincidence G1N-P2-P3
  if (fCopyallHit%105 == 0) man->FillNtupleIColumn(4, 14, NRuns); //coincidence G1B-P2-P3
  if (fCopyallHit%210 == 0) man->FillNtupleIColumn(4, 15, NRuns); //coincidence G1N-G1B-P2-P3
  man->AddNtupleRow(4); //add the tuple at the end

  //  G4cout << "Coincidences seen: " << fCopyallSeen << G4endl;
  //Now, we work with efficiencies to decide whether we accept values or not
  if (fCopyallSeen == 1)     man->FillNtupleIColumn(5, 0, NRuns); //no coincidences
  if (fCopyallSeen%2 == 0)   man->FillNtupleIColumn(5, 1, NRuns); //G1N only
  if (fCopyallSeen%3 == 0)   man->FillNtupleIColumn(5, 2, NRuns); //G1B only
  if (fCopyallSeen%5 == 0)   man->FillNtupleIColumn(5, 3, NRuns); //P2 only
  if (fCopyallSeen%7 == 0)   man->FillNtupleIColumn(5, 4, NRuns); //P3 only
  if (fCopyallSeen%6 == 0)   man->FillNtupleIColumn(5, 5, NRuns); //coincidence G1N-G1B
  if (fCopyallSeen%10 == 0)  man->FillNtupleIColumn(5, 6, NRuns); //coincidence G1N-P2
  if (fCopyallSeen%14 == 0)  man->FillNtupleIColumn(5, 7, NRuns); //coincidence G1N-P3
  if (fCopyallSeen%15 == 0)  man->FillNtupleIColumn(5, 8, NRuns); //coincidence G1B-P2
  if (fCopyallSeen%21 == 0)  man->FillNtupleIColumn(5, 9, NRuns); //coincidence G1B-P3
  if (fCopyallSeen%35 == 0)  man->FillNtupleIColumn(5, 10, NRuns); //coincidence P2-P3
  if (fCopyallSeen%30 == 0)  man->FillNtupleIColumn(5, 11, NRuns); //coincidence G1N-G1B-P2
  if (fCopyallSeen%42 == 0)  man->FillNtupleIColumn(5, 12, NRuns); //coincidence G1N-G1B-P3
  if (fCopyallSeen%70 == 0)  man->FillNtupleIColumn(5, 13, NRuns); //coincidence G1N-P2-P3
  if (fCopyallSeen%105 == 0) man->FillNtupleIColumn(5, 14, NRuns); //coincidence G1B-P2-P3
  if ((fCopyallSeen%6 == 0) && (fCopyallHit%5 == 0) )  man->FillNtupleIColumn(5, 15, NRuns); //coincidence G1N-G1B, hit by P2
  if ((fCopyallSeen%6 == 0) && (fCopyallHit%7 == 0) )  man->FillNtupleIColumn(5, 16, NRuns); //coincidence G1N-G1B, hit by P3
  if ((fCopyallSeen%35 == 0) && (fCopyallHit%2 == 0) ) man->FillNtupleIColumn(5, 17, NRuns); //coincidence P2-P3, hit by G1N
  if ((fCopyallSeen%35 == 0) && (fCopyallHit%3 == 0) ) man->FillNtupleIColumn(5, 18, NRuns); //coincidence P2-P3, hit by G1B
  if (fCopyallSeen%210 == 0) man->FillNtupleIColumn(5, 19, NRuns); //coincidence G1N-G1B-P2-P3

  man->AddNtupleRow(5); //add the tuple at the end

  ID++;  //increment the ID
  if (ID%1000 == 0) G4cout << "Event number: " << ID << G4endl;
  if (ID == 1)
    {
      G4cout << G4endl;
      G4cout << "Eff[0] = " << Eff[0] << G4endl;
      G4cout << "Eff[1] = " << Eff[1] << G4endl;
      G4cout << "Eff[2] = " << Eff[2] << G4endl;
      G4cout << "Eff[3] = " << Eff[3] << G4endl;
      G4cout << G4endl;
    }
  
}
