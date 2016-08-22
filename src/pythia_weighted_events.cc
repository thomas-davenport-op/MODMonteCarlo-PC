#include <iostream>
#include <cstdlib>


#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"


#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/SoftDrop.hh"



using namespace Pythia8;

using namespace fastjet;
using namespace contrib;


int main(int argc, char* argv[]) {

  Pythia pythia;

  // Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::Pythia8ToHepMC ToHepMC;
  
  // Read the card file.
  pythia.readFile(argv[1]);

  // Specify file where HepMC events will be stored.
  HepMC::IO_GenEvent ascii_io(argv[2], std::ios::out);

  // Bias high-pT selection (continuously) by a pT^4 factor. Matching also to low-pT processes is more complicated.

  // Shorthand for some public members of pythia (also static ones).
  Settings& settings = pythia.settings;
  Info& info = pythia.info;

  int nEvent = pythia.mode("Main:numberOfEvents");

  // Mode 1: set up pT bins. Keep the last one open-ended.
  double pTlimit[7] = {40., 85., 115., 150., 200., 250., 0.};


  // Mode 2: The whole range in one step, but pT-weighted.
  settings.parm("PhaseSpace:pTHatMin", pTlimit[0]);
  settings.parm("PhaseSpace:pTHatMax", 0.);
  pythia.readString("PhaseSpace:bias2Selection = on");
  pythia.readString("PhaseSpace:bias2SelectionPow = 4.");
  pythia.readString("PhaseSpace:bias2SelectionRef = 10.");

  pythia.init();

  // Begin event loop.
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {

    // Generate events. Skip if failure.

    if ( ! pythia.next()) {
      // If failure because reached end of file then exit event loop.
      if (pythia.info.atEndOfFile()) {
        cout << "Aborted since reached end of event file!!";
        break;
      }
      else {
        continue;
      }
    }

    // Soft events have no upper pT limit. They therefore overlap with hard events, and the overlap must be removed by hand.
    // No overlap for elastic/diffraction, which is only part of soft.
    double pTHat  = info.pTHat();
    
    // Fill hard scale of event.
    double weight = info.weight();

    // Construct new empty HepMC event and fill it.
    // Units will be as chosen for HepMC build, but can be changed
    // by arguments, e.g. GenEvt( HepMC::Units::GEV, HepMC::Units::MM)
    HepMC::GenEvent* hepmcevt = new HepMC::GenEvent();

    // Set event weight
    hepmcevt->weights().push_back(weight);
  

    ToHepMC.fill_next_event( pythia, hepmcevt );


    // End of event loop. Statistics.

    // Write the HepMC event to file. Done with it.
    ascii_io << hepmcevt;
    delete hepmcevt;
  }


  double sigmaNorm = (info.sigmaGen() / info.weightSum());

  cout << "Total Weight Sum = " << info.weightSum() << endl;
  cout << "Total Cross-Section Sum = " << info.sigmaGen() << endl;
  // pythia.stat();




  return 0;
}
