#include "Action.hh"

MyActionInitialization::MyActionInitialization(){
}

MyActionInitialization::~MyActionInitialization(){
}

void MyActionInitialization::Build() const {
  //ready for the action! Set the generator that shoots particle (see Generator.cc)
  MyPrimaryGenerator *generator = new MyPrimaryGenerator();
  SetUserAction(generator);

  MyRunAction *runAction = new MyRunAction();
  SetUserAction(runAction); //from the class that we have derived we tell the generators of events to store data in the root file we want

  //tell Geant4 we also have steps and events in our action to consider
  MyEventAction *eventAction = new MyEventAction(runAction);
  SetUserAction(eventAction);

  MySteppingAction *steppingAction = new MySteppingAction(eventAction);
  SetUserAction(steppingAction);
}
