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

  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();


  // Get pointers to branches used in this analysis.
  TClonesArray *branchJet = treeReader->UseBranch("Jet");
  TClonesArray *branchParticle = treeReader->UseBranch("Particle");

  // Loop over all events.
  for (Int_t entry = 0; entry < numberOfEntries; ++entry) {
    // Load selected branches with data from specified event.
    treeReader->ReadEntry(entry);

    // cout << "Parsing event number: " << (entry + 1) << endl;

    output_stream << "BeginEvent Version 1 Reco Pythia_8212_Delphes_330_Dijet100" << endl;
    
    // Output all jets.
    output_stream << "# MAK5" << "              px              py              pz          energy" << endl;

    for (unsigned i = 0; i < branchJet->GetEntries(); i++) {
      Jet *jet = (Jet*) branchJet->At(i);
      
      // Because 'jet' does not directly provide (px, py, pz, E), write out (pT, eta, phi, m) first and then use that to calculate (px, py, pz, E).
      fastjet::PseudoJet pseudojet = fastjet::PseudoJet(0., 0., 0., 0.);
      pseudojet.reset_PtYPhiM(jet->PT, jet->Eta, jet->Phi, jet->Mass);

      output_stream << "  MAK5"
        << setw(16) << fixed << setprecision(8) << pseudojet.px()
        << setw(16) << fixed << setprecision(8) << pseudojet.py()
        << setw(16) << fixed << setprecision(8) << pseudojet.pz()
        << setw(16) << fixed << setprecision(8) << pseudojet.E()
        << endl;
    }

    // Output all PFCandidates.
    output_stream << "# MPFC" << "              px              py              pz          energy   pdgId" << endl;  


    double total_energy = 0.0;

    for (unsigned i = 0; i < branchParticle->GetEntriesFast(); i++) {
      GenParticle * particle = (GenParticle*) branchParticle->At(i);

      if (particle->Status == 1) {
        output_stream << "  MPFC"
        << setw(16) << fixed << setprecision(8) << particle->Px
        << setw(16) << fixed << setprecision(8) << particle->Py
        << setw(16) << fixed << setprecision(8) << particle->Pz
        << setw(16) << fixed << setprecision(8) << particle->E
        << setw(8) << noshowpos << particle->PID
        << endl;

        total_energy += particle->E;
      }
      
    }

    cout << "Total energy: " << total_energy << endl;

    output_stream << "EndEvent" << endl;
    
  }

  output_file << output_stream.rdbuf();

}

