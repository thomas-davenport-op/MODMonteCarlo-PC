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
  
  TClonesArray *branch_genparticle = tree_reader->UseBranch("GenParticle"); 

  TClonesArray *branch_electron = tree_reader->UseBranch("Electron");
  TClonesArray *branch_photon = tree_reader->UseBranch("Photon");
  TClonesArray *branch_muon = tree_reader->UseBranch("Muon");

  TClonesArray *branch_track = tree_reader->UseBranch("Track");
  TClonesArray *branch_tower = tree_reader->UseBranch("Tower");

  TClonesArray *branch_eflow_track = tree_reader->UseBranch("EFlowTrack");
  TClonesArray *branch_eflow_photon = tree_reader->UseBranch("EFlowPhoton");
  TClonesArray *branch_eflow_neutral_hadron = tree_reader->UseBranch("EFlowNeutralHadron");

  





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

    // Loop over all EFlow tracks
    for (unsigned i = 0; i < branch_eflow_track->GetEntriesFast(); ++i) {
      Track * track = (Track*) branch_eflow_track->At(i);
      TLorentzVector four_vector = track->P4();

      output_stream << "   MPFC"
        << setw(16) << fixed << setprecision(8) << four_vector.Px()
        << setw(16) << fixed << setprecision(8) << four_vector.Py()
        << setw(16) << fixed << setprecision(8) << four_vector.Pz()
        << setw(16) << fixed << setprecision(8) << four_vector.E()
        << setw(8) << noshowpos << track->PID
        << endl;

      pfc_total_energy += four_vector.E();
    }

    // Loop over all EFlow Photons
    for (unsigned i = 0; i < branch_eflow_photon->GetEntriesFast(); ++i) {
      Photon * photon = (Photon*) branch_eflow_photon->At(i);
      TLorentzVector four_vector = photon->P4();

      output_stream << "   MPFC"
        << setw(16) << fixed << setprecision(8) << four_vector.Px()
        << setw(16) << fixed << setprecision(8) << four_vector.Py()
        << setw(16) << fixed << setprecision(8) << four_vector.Pz()
        << setw(16) << fixed << setprecision(8) << four_vector.E()
        << setw(8) << noshowpos << "22"
        << endl;

      pfc_total_energy += four_vector.E();
    }

    // Loop over all EFlow Hadrons
    for (unsigned i = 0; i < branch_eflow_neutral_hadron->GetEntriesFast(); ++i) {

      TObject * object = branch_eflow_neutral_hadron->At(i);
        
      if (object->IsA() == GenParticle::Class()) {
        GenParticle * gen_particle = (GenParticle*) object;

        TLorentzVector four_vector = gen_particle->P4();

        output_stream << "   MPFC"
          << setw(16) << fixed << setprecision(8) << four_vector.Px()
          << setw(16) << fixed << setprecision(8) << four_vector.Py()
          << setw(16) << fixed << setprecision(8) << four_vector.Pz()
          << setw(16) << fixed << setprecision(8) << four_vector.E()
          << setw(8) << noshowpos << gen_particle->PID
          << endl;

        pfc_total_energy += four_vector.E();
      }
      else if(object->IsA() == Tower::Class()) {
        
        Tower * tower = (Tower*) object;

        for (unsigned j = 0; j < tower->Particles.GetEntriesFast(); ++j) {
          GenParticle * gen_particle = (GenParticle*) tower->Particles.At(j);

          TLorentzVector four_vector = gen_particle->P4();

          output_stream << "   MPFC"
            << setw(16) << fixed << setprecision(8) << four_vector.Px()
            << setw(16) << fixed << setprecision(8) << four_vector.Py()
            << setw(16) << fixed << setprecision(8) << four_vector.Pz()
            << setw(16) << fixed << setprecision(8) << four_vector.E()
            << setw(8) << noshowpos << gen_particle->PID
            << endl;

          pfc_total_energy += four_vector.E();
          
        }
      }
      
    }

    cout << "PFC Total Energy: " << pfc_total_energy << endl;

    output_stream << "EndEvent" << endl;
    
  }

  output_file << output_stream.rdbuf();

}

