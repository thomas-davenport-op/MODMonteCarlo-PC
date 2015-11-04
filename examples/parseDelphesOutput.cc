#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "TChain.h"
#include "TTree.h"
#include "TClonesArray.h"


using namespace std;


#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"

#include "external/fastjet/ClusterSequence.hh"


void parseDelphesOutput(const char * inputFile, const char * outputFileName);

int main(int argc, char* argv[]) {

  const char * inputFile = argv[1];
  const char * outputFile = argv[2];

  parseDelphesOutput(inputFile, outputFile);

  return 0;
}



void parseDelphesOutput(const char * inputFile, const char * outputFileName) {


  ofstream output_file(outputFileName, ios::out);
  stringstream output_stream;

  // Create chain of root trees.
  TChain chain("Delphes");
  chain.Add(inputFile);

  ExRootTreeReader * tree_reader = new ExRootTreeReader(&chain);
  Long64_t number_of_entries = tree_reader->GetEntries();


  // Get pointers to branches used in this analysis.
  TClonesArray *branch_jet = tree_reader->UseBranch("Jet");
  TClonesArray *branch_particle = tree_reader->UseBranch("Particle");  // This is for Truth Particles.

  TClonesArray *branch_track = tree_reader->UseBranch("EFlowTrack");
  TClonesArray *branch_photon = tree_reader->UseBranch("EFlowPhoton");
  TClonesArray *branch_neutral_hadron = tree_reader->UseBranch("EFlowNeutralHadron");

  // Loop over all events.
  for (Int_t entry = 0; entry < number_of_entries; ++entry) {
    // Load selected branches with data from specified event.
    tree_reader->ReadEntry(entry);

    // cout << "Parsing event number: " << (entry + 1) << endl;

    output_stream << "BeginEvent Version 1 TruthPlusReco Pythia_8212_Delphes_330_Dijet100" << endl;
    
    // Output all jets.
    output_stream << "#  MAK5" << "              px              py              pz          energy" << endl;

    for (unsigned i = 0; i < branch_jet->GetEntries(); i++) {
      Jet *jet = (Jet*) branch_jet->At(i);
      
      // Because 'jet' does not directly provide (px, py, pz, E), write out (pT, eta, phi, m) first and then use that to calculate (px, py, pz, E).
      fastjet::PseudoJet pseudojet = fastjet::PseudoJet(0., 0., 0., 0.);
      pseudojet.reset_PtYPhiM(jet->PT, jet->Eta, jet->Phi, jet->Mass);

      output_stream << "   MAK5"
        << setw(16) << fixed << setprecision(8) << pseudojet.px()
        << setw(16) << fixed << setprecision(8) << pseudojet.py()
        << setw(16) << fixed << setprecision(8) << pseudojet.pz()
        << setw(16) << fixed << setprecision(8) << pseudojet.E()
        << endl;
    }

    // Output all truth particles.
    output_stream << "# TRUTH" << "              px              py              pz          energy   pdgId" << endl;  

    double total_energy = 0.0;

    for (unsigned i = 0; i < branch_particle->GetEntriesFast(); i++) {
      GenParticle * particle = (GenParticle*) branch_particle->At(i);

      if (particle->Status == 1) {
        output_stream << "  TRUTH"
        << setw(16) << fixed << setprecision(8) << particle->Px
        << setw(16) << fixed << setprecision(8) << particle->Py
        << setw(16) << fixed << setprecision(8) << particle->Pz
        << setw(16) << fixed << setprecision(8) << particle->E
        << setw(8) << noshowpos << particle->PID
        << endl;

        total_energy += particle->E;
      }
      
    }

    cout << "Total truth energy: " << total_energy << endl;

    // Finally, PFCs.
    output_stream << "#  MPFC" << "              px              py              pz          energy   pdgId" << endl;
    
    double pfc_total_energy = 0.0;

    for (unsigned i = 0; i < branch_track->GetEntriesFast(); i++) {
      GenParticle * track = (GenParticle*) branch_track->At(i);

      if (track->Status == 1) {
        output_stream << "   MPFC"
        << setw(16) << fixed << setprecision(8) << track->Px
        << setw(16) << fixed << setprecision(8) << track->Py
        << setw(16) << fixed << setprecision(8) << track->Pz
        << setw(16) << fixed << setprecision(8) << track->E
        << setw(8) << noshowpos << track->PID
        << endl;

        pfc_total_energy += track->E;
      }
    }

    for (unsigned i = 0; i < branch_photon->GetEntriesFast(); i++) {
      GenParticle * photon = (GenParticle*) branch_photon->At(i);

      if (photon->Status == 1) {
        output_stream << "   MPFC"
        << setw(16) << fixed << setprecision(8) << photon->Px
        << setw(16) << fixed << setprecision(8) << photon->Py
        << setw(16) << fixed << setprecision(8) << photon->Pz
        << setw(16) << fixed << setprecision(8) << photon->E
        << setw(8) << noshowpos << photon->PID
        << endl;

        pfc_total_energy += photon->E;
      }
    }

    for (unsigned i = 0; i < branch_neutral_hadron->GetEntriesFast(); i++) {
      GenParticle * neutral_hadron = (GenParticle*) branch_neutral_hadron->At(i);

      if (neutral_hadron->Status == 1) {
        output_stream << "   MPFC"
        << setw(16) << fixed << setprecision(8) << neutral_hadron->Px
        << setw(16) << fixed << setprecision(8) << neutral_hadron->Py
        << setw(16) << fixed << setprecision(8) << neutral_hadron->Pz
        << setw(16) << fixed << setprecision(8) << neutral_hadron->E
        << setw(8) << noshowpos << neutral_hadron->PID
        << endl;

        pfc_total_energy += neutral_hadron->E;
      }
    }

    cout << "PFC Total Energy: " << pfc_total_energy << endl;

    output_stream << "EndEvent" << endl;
    
  }

  output_file << output_stream.rdbuf();

}

