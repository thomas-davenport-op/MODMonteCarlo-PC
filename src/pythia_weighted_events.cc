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

  // 1 : Using the Main:subrun keyword in a separate command file.
  // 2 : Bias high-pT selection (continuously) by a pT^4 factor. Matching also to low-pT processes is more complicated.
  int mode = atoi( argv[3] ); // Treats any invalid number as 0.
  

  // Shorthand for some public members of pythia (also static ones).
  Settings& settings = pythia.settings;
  Info& info = pythia.info;

  int nEvent = pythia.mode("Main:numberOfEvents");


  // One does not need complete events to study pThard spectrum only.
  bool completeEvents = false;

  // Number of bins to use. In mode 2 read from the card file.
  int nBin = 1;
  if      (mode == 1)  nBin = pythia.mode("Main:numberOfSubruns");
  else if (mode == 2)  nBin = 1;

  // Mode 1: set up five pT bins - last one open-ended.
  double pTlimit[8] = {85., 115., 150., 200., 250., 500., 750., 0.};


  // Loop over number of bins, i.e. number of subruns.
  for (int iBin = 0; iBin < nBin; ++iBin) {

    if (mode == 1) {                                           // Mode 1: subruns stored in the card file.
      pythia.readFile(argv[1], iBin);
    }
    else if (mode == 2) {                                     // Mode 2: The whole range in one step, but pT-weighted.
      settings.parm("PhaseSpace:pTHatMin", pTlimit[0]);
      settings.parm("PhaseSpace:pTHatMax", 0.);
      pythia.readString("PhaseSpace:bias2Selection = on");
      pythia.readString("PhaseSpace:bias2SelectionPow = 4.");
      pythia.readString("PhaseSpace:bias2SelectionRef = 85.");
    }

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

      // Soft events have no upper pT limit. They therefore overlap
      // with hard events, and the overlap must be removed by hand.
      // No overlap for elastic/diffraction, which is only part of soft.
      double pTHat  = info.pTHat();
      
      // Fill hard scale of event.
      double weight = info.weight();

      // Construct new empty HepMC event and fill it.
      // Units will be as chosen for HepMC build, but can be changed
      // by arguments, e.g. GenEvt( HepMC::Units::GEV, HepMC::Units::MM)
      HepMC::GenEvent* hepmcevt = new HepMC::GenEvent();

      // Set event weight
      if (mode == 1) {
        hepmcevt->weights().push_back(weight);  
        // cout << weight << endl;
      }
      else {
       hepmcevt->weights().push_back(weight * pow3(pTHat));   
       // cout << weight * pow3(pTHat) << endl;
      }
      

      ToHepMC.fill_next_event( pythia, hepmcevt );


      // pTraw.fill( pTHat );
      // pTnormPart.fill( pTHat, weight);
      // pTpow3Part.fill( pTHat, weight * pow3(pTHat) );

      // Report cross section to hepmc
      // HepMC::GenCrossSection xsec;
      // xsec.set_cross_section(  );
      // hepmcevt->set_cross_section( xsec );

      // End of event loop. Statistics.

      // Write the HepMC event to file. Done with it.
      ascii_io << hepmcevt;
      delete hepmcevt;
    }

    // pythia.stat();

    // End of pT-bin loop.
  }


  return 0;
}