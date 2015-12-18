#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>


#include "HepMC/IO_GenEvent.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/SoftDrop.hh"


using namespace std;
using namespace HepMC;



void parseHepMC(string input_file_name, string output_file_name);


int main(int argc, char* argv[]) {

  string input_file_name = argv[1];
  string output_file_name = argv[2];

  parseHepMC(input_file_name, output_file_name);
  // zg_stuff();

  return 0;
}


void parseHepMC(string input_file_name, string output_file_name) {

  ofstream output_file(output_file_name, ios::out);

  HepMC::IO_GenEvent ascii_in(input_file_name, std::ios::in);
  stringstream output_stream;
  
  int icount = 0;
  HepMC::GenEvent* event = ascii_in.read_next_event();

  while (event) {
  
    icount++;
    
    if (icount % 100 == 0)
      std::cout << "Processing Event Number " << icount << std::endl;
    
    // cout << "Number of particles: " << event->particles_size() << endl;
    // cout << "Number of vertices: " << event->vertices_size() << endl;

    GenEvent::particle_const_iterator particle = event->particles_begin();

    HepMC::GenEvent::particle_const_iterator it; 

    output_stream << "BeginEvent Version 1 Truth Pythia_8212_Dijet100" << endl;
    
    
    double net_px = 0;
    double net_py = 0;
    double net_pz = 0;
    double net_energy = 0;

    std::vector<fastjet::PseudoJet> truth_particles;

    for ( it = event->particles_begin(); it != event->particles_end(); it++ ) {
      
      if (( ! ( * it)->is_beam() ) && (( * it)->status() == 1)) {
        
        net_px += ( * it)->momentum().px();
        net_py += ( * it)->momentum().py();
        net_pz += ( * it)->momentum().pz();
        net_energy += ( * it)->momentum().e();


        fastjet::PseudoJet pseudojet = fastjet::PseudoJet(( * it)->momentum().px(), ( * it)->momentum().py(), ( * it)->momentum().pz(), ( * it)->momentum().e());
        pseudojet.set_user_index( ( * it)->pdg_id());
        truth_particles.push_back(pseudojet);
        
      }
      
    }

    truth_particles = sorted_by_pt(truth_particles);

    if ( abs(net_energy - 7000) > 1 ) {
      cout << "ERROR:: Energy not conserved- " << net_energy << " vs. 7000!" << endl;
      // throw std::runtime_error( "ERROR:: Energy not conserved!" );
    }
    
    if ((abs(net_px) > 1e-3) || (abs(net_py) > 1e-3) || (abs(net_pz) > 1e-3)) {
      cout << "ERROR:: Momentum not conserved!" << endl;
      // throw std::runtime_error( "ERROR:: Momentum not conserved!" );
    }

    // Cluster all truth particles.
    fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, 0.5);
    fastjet::ClusterSequence cs(truth_particles, jet_def);
    std::vector<fastjet::PseudoJet> truth_jets = sorted_by_pt(cs.inclusive_jets(0.0));
    
    // // Output TAK5 jets.
    // output_stream << "#  TAK5" << "              px              py              pz          energy" << endl;
    // for (unsigned i = 0; i < truth_jets.size(); i++) {
    //   output_stream << "   TAK5"
    //     << setw(16) << fixed << setprecision(8) << truth_jets[i].px()
    //     << setw(16) << fixed << setprecision(8) << truth_jets[i].py()
    //     << setw(16) << fixed << setprecision(8) << truth_jets[i].pz()
    //     << setw(16) << fixed << setprecision(8) << truth_jets[i].E()
    //     << endl;
    // }

    // Next, output TPFCs.
    output_stream << "# TRUTH" << "               px              py              pz          energy   pdgId" << endl;  
    for (unsigned i = 0; i < truth_particles.size(); i++) {
      output_stream << "  TRUTH "
        << setw(16) << fixed << setprecision(8) << truth_particles[i].px()
        << setw(16) << fixed << setprecision(8) << truth_particles[i].py()
        << setw(16) << fixed << setprecision(8) << truth_particles[i].pz()
        << setw(16) << fixed << setprecision(8) << truth_particles[i].E()
        << setw(8) << noshowpos << truth_particles[i].user_index()
        << endl;
    }


    output_stream << "EndEvent" << endl;
    
    delete event;
    ascii_in >> event;
  }


  output_file << output_stream.rdbuf();

  std::cout << icount << " events found. And I'm done." << std::endl;
}





