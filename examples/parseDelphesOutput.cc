/*
Simple macro showing how to access branches from the delphes output root file,
loop over events, and plot simple quantities such as the jet pt and the di-electron invariant
mass.

root -l examples/parseDelphesOutput.C'("delphes_output.root")'
*/

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#endif

//------------------------------------------------------------------------------

void parseDelphesOutput(const char * inputFile, const char * outputFileName) {


  ofstream output_file(outputFileName, ios::out);


  gSystem->Load("libDelphes");

  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFile);

  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();

  // Get pointers to branches used in this analysis
  TClonesArray *branchJet = treeReader->UseBranch("Jet");

  TClonesArray *branchParticle = treeReader->UseBranch("Particle");

  // Loop over all events
  for (Int_t entry = 0; entry < numberOfEntries; ++entry) {
    // Load selected branches with data from specified event
    treeReader->ReadEntry(entry);

    if (branchJet->GetEntries() > 0) {
      Jet *jet = (Jet*) branchJet->At(0);

      cout << "Jet pt: " << jet->PT << endl;

      output_file << jet->PT << endl;

    }
    
  }

}

