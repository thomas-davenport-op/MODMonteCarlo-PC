#include <stdexcept>
#include <iostream>
#include <sstream>

#include <fstream>
#include <string>

#include <vector>

#include <signal.h>

#include "TROOT.h"
#include "TApplication.h"

#include "TFile.h"
#include "TObjArray.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TLorentzVector.h"

#include "modules/Delphes.h"
#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "classes/DelphesHepMCReader.h"

// Added later.
#include "modules/ParticlePropagator.h"
#include "external/ExRootAnalysis/ExRootConfReader.h"

#include "external/ExRootAnalysis/ExRootTreeWriter.h"
#include "external/ExRootAnalysis/ExRootTreeBranch.h"
#include "external/ExRootAnalysis/ExRootProgressBar.h"



#include "HepMC/IO_GenEvent.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequence.hh"

using namespace std;
using namespace fastjet;
using namespace HepMC;


static bool interrupted = false;

void SignalHandler(int sig) {
  interrupted = true;
}

//---------------------------------------------------------------------------

int main(int argc, char *argv[]) {
  char app_name[] = "ExternalFastJetHepMC";
  stringstream message;
  FILE *input_file = 0;
  ExRootConfReader *conf_reader = 0;
  Delphes *modular_delphes = 0;
  DelphesFactory *factory = 0;
  TObjArray *stable_particle_output_array = 0, *all_particle_output_array = 0, *parton_output_array = 0;
  DelphesHepMCReader *reader = 0;
  Int_t i, max_events, skip_events;
  Long64_t length, event_counter;
  
  TObjArray *input_array = 0;
  TIterator *input_iterator = 0;
  Candidate *candidate = 0;
  Candidate *truth_candidate = 0;
  TLorentzVector momentum;

  JetDefinition *definition = 0;
  vector<PseudoJet> input_list, output_list, truth_pfcs, truth_jets;
  PseudoJet jet;

  string card_filename = argv[1];
  string input_filename = argv[2];
  string output_filename = input_filename + ".mod";

  ofstream output_file(output_filename.c_str(), ios::out);
  stringstream output_stream;

  if(argc < 2) {
    cout << " Usage: " << app_name << " config_file" << " [input_file(s)]" << endl;
    cout << " config_file - configuration file in Tcl format," << endl;
    cout << " input_file(s) - input file(s) in HepMC format," << endl;
    cout << " with no input_file, or when input_file is -, read standard input." << endl;
    return 1;
  }

  signal(SIGINT, SignalHandler);

  gROOT->SetBatch();

  int appargc = 1;
  char *appargv[] = {app_name};
  TApplication app(app_name, &appargc, appargv);

  try {
    conf_reader = new ExRootConfReader;
    conf_reader->ReadFile(card_filename.c_str());

    max_events = conf_reader->GetInt("::Max_events", 0);
    skip_events = conf_reader->GetInt("::Skip_events", 0);

    if(max_events < 0) {
      throw runtime_error("Max_events must be zero or positive");
    }

    if(skip_events < 0) {
      throw runtime_error("Skip_events must be zero or positive");
    }

    modular_delphes = new Delphes("Delphes");
    modular_delphes->SetConfReader(conf_reader);

    factory = modular_delphes->GetFactory();
    all_particle_output_array = modular_delphes->ExportArray("allParticles");
    stable_particle_output_array = modular_delphes->ExportArray("stableParticles");
    parton_output_array = modular_delphes->ExportArray("partons");

    // TObjArray * abc = modular_delphes->ExportArray("InputArray",  "Delphes/stableParticles");




    reader = new DelphesHepMCReader;

    modular_delphes->InitTask();

    ClusterSequence::print_banner();

  
    definition = new JetDefinition(antikt_algorithm, 0.5);
    
    
    // Define your input candidates to fastjet (by default particle-flow objects).
    // If you want pure calorimeter towers change "EFlowMerger/eflow" into "Calorimeter/towers":
     
    input_array = modular_delphes->ImportArray("EFlowMerger/eflow");

    TObjArray * stable_particle_input_array = modular_delphes->ImportArray("Delphes/stableParticles");
    TIterator * stable_particle_iterator = stable_particle_input_array->MakeIterator();


    input_iterator = input_array->MakeIterator();


    // start reading hepmc file

    i = 2;
    
    do {
      if(interrupted) break;

      if(i == argc || strncmp(argv[i], "-", 2) == 0) {
        cout << "** Reading standard input" << endl;
        input_file = stdin;
        length = -1;
      }
      else {
        cout << "** Reading " << argv[i] << endl;
        input_file = fopen(argv[i], "r");

        if(input_file == NULL) {
          message << "can't open " << argv[i];
          throw runtime_error(message.str());
        }

        fseek(input_file, 0L, SEEK_END);
        length = ftello(input_file);
        fseek(input_file, 0L, SEEK_SET);

        if(length <= 0) {
          fclose(input_file);
          ++i;
          continue;
        }
      }

      
      reader->SetInputFile(input_file);

      // HepMC::IO_GenEvent ascii_in(input_file, std::ios::in);

      // Loop over all objects
      event_counter = 0;
      modular_delphes->Clear();
      reader->Clear();
      while((max_events <= 0 || event_counter - skip_events < max_events) && reader->ReadBlock(factory, all_particle_output_array, stable_particle_output_array, parton_output_array) && !interrupted) {
	
      	// loop over events
      	if(reader->EventReady()) {
          ++event_counter;

          if (event_counter > skip_events) {

      	    // run delphes reconstruction
      	    modular_delphes->ProcessTask();
                  
            input_list.clear();
            input_iterator->Reset();

            truth_pfcs.clear();
            stable_particle_iterator->Reset();
      	
            output_stream << "BeginEvent Version 1 TruthPlusReco Pythia_8212_Delphes_330_Dijet100" << endl;


            // Add all truth particles into a vector. We'll cluster this and write out TAK5 for now. We'll write the truth particles themselves later.
            while((truth_candidate = static_cast<Candidate*>(stable_particle_iterator->Next()))) {
              momentum = truth_candidate->Momentum;
              jet = PseudoJet(momentum.Px(), momentum.Py(), momentum.Pz(), momentum.E());
              jet.set_user_index(truth_candidate->PID);
              if (truth_candidate->Status == 1) {
                truth_pfcs.push_back(jet);
              }
            }
            truth_pfcs = sorted_by_pt(truth_pfcs);

            // Cluster it with FastJet.
            ClusterSequence cs(truth_pfcs, *definition);
            truth_jets.clear();
            truth_jets = sorted_by_pt(cs.inclusive_jets(0.0));

            // Write out truth jets.
            output_stream << "#  TAK5" << "              px              py              pz          energy" << endl;
            for (unsigned k = 0; k < truth_jets.size(); k++) {
              output_stream << "   TAK5"
                            << setw(16) << fixed << setprecision(8) << truth_jets[k].px()
                            << setw(16) << fixed << setprecision(8) << truth_jets[k].py()
                            << setw(16) << fixed << setprecision(8) << truth_jets[k].pz()
                            << setw(16) << fixed << setprecision(8) << truth_jets[k].E()
                            << endl;
            }


      	    // pass delphes candidates to fastjet clustering  
            while((candidate = static_cast<Candidate*>(input_iterator->Next()))) {
              momentum = candidate->Momentum;
              jet = PseudoJet(momentum.Px(), momentum.Py(), momentum.Pz(), momentum.E());
              jet.set_user_index(candidate->PID);
              if ( (candidate->Status == 1) && ( abs(jet.eta()) < 5.0 ) ) {
                input_list.push_back(jet);
              }
            }
            input_list = sorted_by_pt(input_list);

      	    // run fastjet clustering 
      	    ClusterSequence sequence(input_list, *definition);
            output_list.clear();
            output_list = sorted_by_pt(sequence.inclusive_jets(0.0));

            // Write out reconstructed AK5 jets.
            output_stream << "#  RAK5" << "              px              py              pz          energy" << endl;

            for (unsigned k = 0; k < output_list.size(); k++) {
                output_stream << "   RAK5"
                            << setw(16) << fixed << setprecision(8) << output_list[k].px()
                            << setw(16) << fixed << setprecision(8) << output_list[k].py()
                            << setw(16) << fixed << setprecision(8) << output_list[k].pz()
                            << setw(16) << fixed << setprecision(8) << output_list[k].E()
                            << endl;
            }

            // Next, truth particles.
            output_stream << "# TRUTH" << "              px              py              pz          energy   pdgId" << endl;
            for (unsigned k = 0; k < truth_pfcs.size(); k++) {
              output_stream << "  TRUTH"
                            << setw(16) << fixed << setprecision(8) << truth_pfcs[k].px()
                            << setw(16) << fixed << setprecision(8) << truth_pfcs[k].py()
                            << setw(16) << fixed << setprecision(8) << truth_pfcs[k].pz()
                            << setw(16) << fixed << setprecision(8) << truth_pfcs[k].E()
                            << setw(8) << noshowpos << truth_pfcs[k].user_index()
                            << endl;
            }

            // Finally, reconstructed "PFCandidates".
            output_stream << "#  RPFC" << "              px              py              pz          energy   pdgId" << endl;

            for (unsigned k = 0; k < input_list.size(); k++) {
              output_stream << "   RPFC"
                            << setw(16) << fixed << setprecision(8) << input_list[k].px()
                            << setw(16) << fixed << setprecision(8) << input_list[k].py()
                            << setw(16) << fixed << setprecision(8) << input_list[k].pz()
                            << setw(16) << fixed << setprecision(8) << input_list[k].E()
                            << setw(8) << noshowpos << input_list[k].user_index()
                            << endl;
            }

            output_stream << "EndEvent" << endl;

          }

          modular_delphes->Clear();
          reader->Clear();
        }
      } // end of event loop

      if(input_file != stdin) fclose(input_file);

      ++i;
    }
    while(i < argc);

    modular_delphes->FinishTask();

    output_file << output_stream.rdbuf();

    cout << "** Exiting..." << endl;

    delete definition;

    delete reader;
    delete modular_delphes;
    delete conf_reader;


    return 0;
  }
  catch(runtime_error &e){
    cerr << "** ERROR: " << e.what() << endl;
    return 1;
  }



}