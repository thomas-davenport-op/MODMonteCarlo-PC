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
  
  TClonesArray *branch_eflow_track = tree_reader->UseBranch("EFlowTrack");
  TClonesArray *branch_eflow_photon = tree_reader->UseBranch("EFlowPhoton");
  TClonesArray *branch_eflow_neutral_hadron = tree_reader->UseBranch("EFlowNeutralHadron");


  // Loop over all events.
  for (Int_t entry = 0; entry < number_of_entries; ++entry) {
    // Load selected branches with data from specified event.
    tree_reader->ReadEntry(entry);

    cout << "Parsing event number: " << (entry + 1) << " \t";


    output_stream << "BeginEvent Version 1 TruthPlusReco Pythia_8212_Delphes_330_Dijet100 Prescale 1" << endl;
    
    // Output all jets.
    output_stream << "#  TAK5" << "              px              py              pz          energy" << endl;


    for (unsigned i = 0; i < branch_jet->GetEntries(); i++) {
      Jet *jet = (Jet*) branch_jet->At(i);

      // Because 'jet' does not directly provide (px, py, pz, E), write out (pT, eta, phi, m) first and then use that to calculate (px, py, pz, E).
      fastjet::PseudoJet pseudojet = fastjet::PseudoJet(0., 0., 0., 0.);
      pseudojet.reset_PtYPhiM(jet->PT, jet->Eta, jet->Phi, jet->Mass);

      output_stream << "   TAK5"
        << setw(16) << fixed << setprecision(8) << pseudojet.px()
        << setw(16) << fixed << setprecision(8) << pseudojet.py()
        << setw(16) << fixed << setprecision(8) << pseudojet.pz()
        << setw(16) << fixed << setprecision(8) << pseudojet.E()
        << endl;

    }

    // Add all particles to a vector. We do this instead of just writing them out because we want to cluster them later on.
    vector<fastjet::PseudoJet> particles;

    // Loop over all EFlow tracks.
    for (unsigned i = 0; i < branch_eflow_track->GetEntriesFast(); ++i) {
      Track * track = (Track*) branch_eflow_track->At(i);
      TLorentzVector four_vector = track->P4();
      fastjet::PseudoJet particle = fastjet::PseudoJet(four_vector.Px(), four_vector.Py(), four_vector.Pz(), four_vector.E());
      particle.set_user_index(track->PID);
      particles.push_back(particle);
    }

    // Loop over all EFlow Photons.
    for (unsigned i = 0; i < branch_eflow_photon->GetEntriesFast(); ++i) {
      Photon * photon = (Photon*) branch_eflow_photon->At(i);
      TLorentzVector four_vector = photon->P4();
      fastjet::PseudoJet particle = fastjet::PseudoJet(four_vector.Px(), four_vector.Py(), four_vector.Pz(), four_vector.E());
      particle.set_user_index(22);
      particles.push_back(particle);
    }

    // Loop over all EFlow Hadrons.
    for (unsigned i = 0; i < branch_eflow_neutral_hadron->GetEntriesFast(); ++i) {

      TObject * object = branch_eflow_neutral_hadron->At(i);
        
      if (object->IsA() == GenParticle::Class()) {
        GenParticle * gen_particle = (GenParticle*) object;
        TLorentzVector four_vector = gen_particle->P4();
        fastjet::PseudoJet particle = fastjet::PseudoJet(four_vector.Px(), four_vector.Py(), four_vector.Pz(), four_vector.E());
        particle.set_user_index(gen_particle->PID);
        particles.push_back(particle);

      }
      else if(object->IsA() == Tower::Class()) {
        
        Tower * tower = (Tower*) object;

        for (unsigned j = 0; j < tower->Particles.GetEntriesFast(); ++j) {
          GenParticle * gen_particle = (GenParticle*) tower->Particles.At(j);
          TLorentzVector four_vector = gen_particle->P4();
          fastjet::PseudoJet particle = fastjet::PseudoJet(four_vector.Px(), four_vector.Py(), four_vector.Pz(), four_vector.E());
          particle.set_user_index(gen_particle->PID);
          particles.push_back(particle);
        }
      }
    }


    // Now that we have all the "particles" we need, cluster them into AK5 jets.
    fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, 0.5);
    fastjet::ClusterSequence cs(particles, jet_def);
    std::vector<fastjet::PseudoJet> jets = cs.inclusive_jets(0.0);

    // Now write those jets out.
    output_stream << "#  RAK5" << "              px              py              pz          energy" << endl;
    for (unsigned i = 0; i < jets.size(); i++) {
      output_stream << "   RAK5"
        << setw(16) << fixed << setprecision(8) << jets[i].px()
        << setw(16) << fixed << setprecision(8) << jets[i].py()
        << setw(16) << fixed << setprecision(8) << jets[i].pz()
        << setw(16) << fixed << setprecision(8) << jets[i].E()
        << endl;
    }

    // Now, write out all Truth particles.
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


    // Finally, reconstructed particles. Because we've already stored all of them in a vector, we just loop through that.
    output_stream << "#  RPFC" << "              px              py              pz          energy   pdgId" << endl;

    for (unsigned i = 0; i < particles.size(); ++i) {
      
      fastjet::PseudoJet particle = particles[i];

      output_stream << "   RPFC"
        << setw(16) << fixed << setprecision(8) << particle.px()
        << setw(16) << fixed << setprecision(8) << particle.py()
        << setw(16) << fixed << setprecision(8) << particle.pz()
        << setw(16) << fixed << setprecision(8) << particle.E()
        << setw(8) << noshowpos << particle.user_index()
        << endl;

    }

    output_stream << "EndEvent" << endl;
    
  }

  output_file << output_stream.rdbuf();

}

