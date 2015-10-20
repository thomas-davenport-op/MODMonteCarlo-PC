#include <iostream>
#include <iomanip>
#include <sstream>

#include "HepMC/IO_GenEvent.h"


using namespace std;
using namespace HepMC;

int main(int argc, char* argv[]) {

  string input_file_name = argv[1];
  string output_file_name = argv[2];

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
    
    output_stream << "#TPFC" << "              px              py              pz          energy   pdgId" << endl;  
    
    for ( it = event->particles_begin(); it != event->particles_end(); it++ ) {
      output_stream << " TPFC"
        << setw(16) << fixed << setprecision(8) << ( * it)->momentum().px()
        << setw(16) << fixed << setprecision(8) << ( * it)->momentum().py()
        << setw(16) << fixed << setprecision(8) << ( * it)->momentum().pz()
        << setw(16) << fixed << setprecision(8) << ( * it)->momentum().e()
        << setw(8) << noshowpos << ( * it)->pdg_id()
        << endl;
    }
    
    output_stream << "EndEvent" << endl;
    
    delete event;
    ascii_in >> event;
  }


  output_file << output_stream.rdbuf();

  std::cout << icount << " events found. And I'm done." << std::endl;
  

  return 0;
}