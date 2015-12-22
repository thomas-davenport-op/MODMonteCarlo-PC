#include <iostream>
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

  // Shorthand for the event record in pythia.
  Event& event = pythia.event;
  // Write out zg information.
  ofstream output_file("/home/aashish/zg_from_pythia.dat", ios::out);
  stringstream output;
  
  // Read the card file.
  pythia.readFile(argv[1]);

  // Specify file where HepMC events will be stored.
  HepMC::IO_GenEvent ascii_io(argv[2], std::ios::out);

  pythia.init();
  
  int nEvent = pythia.mode("Main:numberOfEvents");

  // Begin event loop.
  int iAbort = 0;
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {

    // Generate event.
    if ( ! pythia.next()) {

      // If failure because reached end of file then exit event loop.
      if (pythia.info.atEndOfFile()) {
        cout << "Aborted since reached end of event file!!";
        break;
      }

    }



    // Loop over particles.
    vector<fastjet::PseudoJet> all_particles;
    for (int i = 0; i < event.size(); ++i) if (event[i].isFinal()) {
      all_particles.push_back(fastjet::PseudoJet(event[i].px(), event[i].py(), event[i].pz(), event[i].e()));
    }

    // Cluster all particles.
    JetDefinition jet_def(antikt_algorithm, 0.5);
    ClusterSequence cs(all_particles, jet_def);
    vector<PseudoJet> fastjet_jets = sorted_by_pt(cs.inclusive_jets(0.0));

    // Apply SoftDrop on the hardest jet.
    SoftDrop soft_drop_1(0.0, 0.1);
    PseudoJet soft_drop_jet_1 = soft_drop_1(fastjet_jets[0]);
    double zg_1 = soft_drop_jet_1.structure_of<SoftDrop>().symmetry();

    // cout << zg_1 << endl;
    output << fastjet_jets[0].pt() << "  " << zg_1 << endl;


    // Construct new empty HepMC event and fill it.
    // Units will be as chosen for HepMC build, but can be changed
    // by arguments, e.g. GenEvt( HepMC::Units::GEV, HepMC::Units::MM)
    HepMC::GenEvent* hepmcevt = new HepMC::GenEvent();
    ToHepMC.fill_next_event( pythia, hepmcevt );

    // Write the HepMC event to file. Done with it.
    ascii_io << hepmcevt;
    delete hepmcevt;

  // End of event loop. Statistics.
  }

  // pythia.stat();
  
  

  // ofstream output_file("output.dat", ios::out | ios::app);
  
  output_file << output.rdbuf();

  return 0;
}