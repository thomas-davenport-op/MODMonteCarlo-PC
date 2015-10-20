#include <iostream>


#include "HepMC/IO_GenEvent.h"


using namespace std;
using namespace HepMC;

int main(int argc, char* argv[]) {

  ifstream file_stream_to_parse(argv[1]);
  istream & parse_stream = file_stream_to_parse;



  // IO_GenEvent gen_event(file_stream_to_parse);
  // int icount=0;
  // HepMC::GenEvent* evt = gen_event.read_next_event();

  HepMC::IO_GenEvent ascii_out("2.dat", std::ios::out);

  HepMC::IO_GenEvent ascii_in("output.dat", std::ios::in);
  
  int icount = 0;
  HepMC::GenEvent* evt = ascii_in.read_next_event();


  cout << "Strating things!" << endl;
  
  while ( evt ) {
  
    icount++;
  
    std::cout << "Processing Event Number " << icount << " its # " << evt->event_number()  << std::endl;
    
    // write the event out to the ascii file
    ascii_out << evt;
    delete evt;
    ascii_in >> evt;
  }

  std::cout << icount << " events found. Finished." << std::endl;
  

  return 0;
}