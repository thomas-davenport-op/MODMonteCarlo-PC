#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>


#include "HepMC/IO_GenEvent.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/SoftDrop.hh"


using namespace std;
using namespace HepMC;




bool pseudojets_compare(fastjet::PseudoJet a, fastjet::PseudoJet b);

void parseHepMC(string input_file_name, string output_file_name);

void zg_stuff();

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
  
    std::cout << "Processing Event Number " << icount << std::endl;
    
    cout << "Number of particles: " << event->particles_size() << endl;
    cout << "Number of vertices: " << event->vertices_size() << endl;

    GenEvent::particle_const_iterator particle = event->particles_begin();

    HepMC::GenEvent::particle_const_iterator it; 

    output_stream << "BeginEvent Version 1 Pythia_8212 TruthParticles" << endl;
    
    output_stream << "# TPFC" << "              px              py              pz          energy   pdgId" << endl;  
    

    double net_px = 0;
    double net_py = 0;
    double net_pz = 0;
    double net_energy = 0;

    for ( it = event->particles_begin(); it != event->particles_end(); it++ ) {
      
      if (( ! ( * it)->is_beam() ) && (( * it)->status() == 1)) {
        
        net_px += ( * it)->momentum().px();
        net_py += ( * it)->momentum().py();
        net_pz += ( * it)->momentum().pz();
        net_energy += ( * it)->momentum().e();

        output_stream << "  TPFC"
        << setw(16) << fixed << setprecision(8) << ( * it)->momentum().px()
        << setw(16) << fixed << setprecision(8) << ( * it)->momentum().py()
        << setw(16) << fixed << setprecision(8) << ( * it)->momentum().pz()
        << setw(16) << fixed << setprecision(8) << ( * it)->momentum().e()
        << setw(8) << noshowpos << ( * it)->pdg_id()
        << endl;
      }
      
    }

    if ( abs(net_energy - 7000) > 1 ) {
      cout << "ERROR:: Energy not conserved!" << endl;
      throw std::runtime_error( "ERROR:: Energy not conserved!" );
    }
    
    if ((abs(net_px) > 1e-3) || (abs(net_py) > 1e-3) || (abs(net_pz) > 1e-3)) {
      cout << "ERROR:: Momentum not conserved!" << endl;
      throw std::runtime_error( "ERROR:: Momentum not conserved!" );
    }

    output_stream << "EndEvent" << endl;
    
    delete event;
    ascii_in >> event;
  }


  output_file << output_stream.rdbuf();

  std::cout << icount << " events found. And I'm done." << std::endl;
}



void zg_stuff() {

  ifstream data_stream("data/pythia_mod.dat");
  ofstream output_stream("data/zg_output.dat", ios::out);

  std::vector<std::vector<fastjet::PseudoJet>> events;
  std::vector<fastjet::PseudoJet> particles;

  string line;
  while(getline(data_stream, line)) {
    istringstream iss(line);

    int version;
    string tag, version_keyword, a, b;

    iss >> tag;      
    istringstream stream(line);

   

    if (tag == "TPFC") {
      try {

        string tag;
        double px, py, pz, energy;
        int pdgId;

        iss >> tag >> px >> py >> pz >> energy >> pdgId;

        particles.push_back(fastjet::PseudoJet(px, py, pz, energy));

        // cout << particles.size() << endl;
        // cout << "Adding a particle." << endl;
      }
      catch (exception& e) {
        throw runtime_error("Invalid file format PFC! Something's wrong with the way PFCs have been written.");
      }
    }
    else if (tag == "EndEvent") {
      // cout << "End of Event!" << endl;
      events.push_back(particles);
      // cout << "particles before: " << particles.size() << endl;
      particles.clear(); 
      // cout << "particles after:  " << particles.size() << endl;
    }
  }


  fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, 0.5);

  for (int i = 0; i < events.size(); i++) {

  
    fastjet::ClusterSequence cs(events[i], jet_def);
    std::vector<fastjet::PseudoJet> jets = cs.inclusive_jets(150);
    

    sort(jets.begin(), jets.end(), pseudojets_compare);

    cout << jets[0].E() << endl;

    fastjet::contrib::SoftDrop soft_drop(0.0, 0.05);
    fastjet::PseudoJet soft_drop_jet = soft_drop(jets[0]);
    double zg_05 = soft_drop_jet.structure_of<fastjet::contrib::SoftDrop>().symmetry();

    output_stream << zg_05 << endl;

  }

}



bool pseudojets_compare(fastjet::PseudoJet a, fastjet::PseudoJet b) {
   if (a.pt() > b.pt())
      return true;
   return false;
}