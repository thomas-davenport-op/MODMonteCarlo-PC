#include <iostream>
#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

using namespace Pythia8;

int main(int argc, char* argv[]) {

  Pythia pythia;
  
  // Read the card file.
  pythia.readFile(argv[1]);

  int nEvent = pythia.mode("Main:numberOfEvents");

  pythia.init();
  

  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    pythia.next();

    for (int i = 0; i < pythia.event.size(); ++i) {
      // cout << "i = " << i << ", id = " << pythia.event[i].id() << endl;
    }

  }


  
  
  

  // ofstream output_file("output.dat", ios::out | ios::app);
  


  return 0;
}