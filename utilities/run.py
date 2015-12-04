from subprocess import call
import os
from time import time
import sys
from collections import defaultdict


def run_analyzer():
	
	def delete_everything():
		call(['rm', 'data/pythia_truth.hepmc', 'data/herwig_truth.hepmc', 'data/sherpa_truth.hepmc'])
		call(['rm', 'data/pythia_reco.hepmc', 'data/herwig_reco.hepmc', 'data/sherpa_reco.hepmc'])

		call(['rm', 'data/pythia_truth.mod', 'data/herwig_truth.mod', 'data/sherpa_truth.mod'])
		call(['rm', 'data/pythia_reco.mod', 'data/herwig_reco.mod', 'data/sherpa_reco.mod'])
		call(['rm', 'herwig_run.log', 'herwig_run.out', 'herwig_run.run', 'herwig_run.tex'])
		
		call(['rm', 'MIG_P+P+_7000_NNPDF_2.db', 'MPI_Cross_Sections.dat', 'Results.db', 'Results.db.bak', 'Sherpa_References.tex'])

	
	def run_mc_programs():
		
		call(['./bin/generateEvents', 'cards/pythia_card.cmnd', 'data/pythia_truth.hepmc'])

		call(['Herwig++', 'read', 'cards/herwig_card.in'])
		call(['Herwig++', 'run', 'herwig_run.run', '-d1'])
		call(['mv', 'herwig_run.hepmc', 'data/herwig_truth.hepmc'])	# Move to correct location.

		call(['Sherpa', '-f', 'cards/sherpa_card.dat'])

		call(['mv', 'sherpa_long_MPI.hepmc', 'data/sherpa_truth.hepmc'])	# Move to correct location.

	def parse_hepmc():
		call(['./bin/parseHepMC', 'data/pythia_truth.hepmc', 'data/pythia_truth.mod'])
		call(['./bin/parseHepMC', 'data/herwig_truth.hepmc', 'data/herwig_truth.mod'])
		call(['./bin/parseHepMC', 'data/sherpa_truth.hepmc', 'data/sherpa_truth.mod'])


	def run_delphes():
		call(['./bin/runDelphes', 'cards/delphes_card_CMS_NoFastJet.tcl', 'data/pythia_truth.hepmc'])
		call(['./bin/runDelphes', 'cards/delphes_card_CMS_NoFastJet.tcl', 'data/herwig_truth.hepmc'])
		call(['./bin/runDelphes', 'cards/delphes_card_CMS_NoFastJet.tcl', 'data/sherpa_truth.hepmc'])

		call(['mv', 'data/pythia_truth.hepmc.mod', 'data/pythia_reco.mod'])
		call(['mv', 'data/herwig_truth.hepmc.mod', 'data/herwig_reco.mod'])
		call(['mv', 'data/sherpa_truth.hepmc.mod', 'data/sherpa_reco.mod'])


	delete_everything()
	
	# Run the three MC programs.
	run_mc_programs()

	parse_hepmc()
	run_delphes()



start = time()

run_analyzer()

end = time()

print "Everything done in " + str(end - start) + " seconds!"