from __future__ import division
from subprocess import call
import os
from time import time
import sys
from collections import defaultdict
import random
import uuid
import math

output_dir = sys.argv[1]
total_number_of_events = int(sys.argv[2])


cards_directory = "./cards"
events_per_file = 100
remove_hepmc_files = True

def get_number_of_events_per_file():
	if total_number_of_events < events_per_file:
		return total_number_of_events
	else:
		return events_per_file

def get_pT_min(mc_program, card_file):
	f = open(card_file, 'r')
  	lines = f.read().split("\n")

  	if mc_program == "pythia":
  		pT_min = 10.
  	elif mc_program == "herwig":
  		pT_min_line = [line for line in lines if 'JetKtCut:MinKT' in line][0]
  		pT_min = float(pT_min_line.split("JetKtCut:MinKT ")[1].split("*GeV")[0])
  	elif mc_program == "sherpa":
  		pT_min_line = [line for line in lines if 'NJetFinder' in line and "# NJetFinder <n> <ptmin>" not in line][0]
  		pT_min = float(pT_min_line.split("NJetFinder ")[1].split(";")[0].split(" ")[1])
  	else:
		raise Exception("Unknown Monte Carlo program enterred! I know only Pythia, Herwig and Sherpa.")


  	return pT_min



def get_card(mc_program, f):
	
	random_seeds = random.sample(range(1000000), 4)
	random_seed = random.randint(0, 1000000)

	number_of_events = get_number_of_events_per_file()

	f = open(f, 'r')
	lines = f.read().split("\n")

	if mc_program == "pythia":
		card_lines = filter(lambda line: "Main:numberOfEvents" not in line and "Random:seed" not in line, lines) + [ "Random:seed = {}\nMain:numberOfEvents = {}".format(random_seed, number_of_events) ]
	elif mc_program == "herwig":
		filtered_card_lines = filter(lambda line: "LHCGenerator:NumberOfEvents" not in line and "LHCGenerator:PrintEvent" not in line and "/Herwig/Analysis/HepMCFile:PrintEvent" not in line and "LHCGenerator:RandomNumberGenerator:Seed" not in line, lines)
		
		index = filtered_card_lines.index("cd /Herwig/Generators")
		card_lines = filtered_card_lines[:index + 1] + ["set LHCGenerator:NumberOfEvents {}\nset LHCGenerator:PrintEvent {}\nset LHCGenerator:RandomNumberGenerator:Seed {}".format(number_of_events, number_of_events, random_seed) ] + filtered_card_lines[index + 1:]

		index = card_lines.index("insert /Herwig/Generators/LHCGenerator:AnalysisHandlers[0] /Herwig/Analysis/HepMCFile")
		card_lines = card_lines[:index + 1] + [ "set /Herwig/Analysis/HepMCFile:PrintEvent {}".format(number_of_events) ] + card_lines[index + 1:]
		
	elif mc_program == "sherpa":
		filtered_card_lines = filter(lambda line: "EVENTS" not in line and "RANDOM_SEED" not in line, lines)
		
		index = filtered_card_lines.index("(run){")

		card_lines = filtered_card_lines[:index + 1] + [ "  EVENTS {}".format(number_of_events) ] + filtered_card_lines[index + 1:]

		index = card_lines.index("  BEAM_2 2212; BEAM_ENERGY_2 3500;")

		card_lines = card_lines[:index + 2] + [ "  RANDOM_SEED{}={}".format( (i + 1), random_seeds[i]) for i in range(4) ] + card_lines[index + 2:]
		
	else:
		raise Exception("Unknown Monte Carlo program enterred! I know only Pythia, Herwig and Sherpa.")

	return "\n".join(map(str, card_lines))


def write_card(content, f):

	with open(f, 'w') as card_file:
		card_file.write(content)

def run_mc_program(mc_program, output_directory, temp_card_file):
	if not os.path.exists(output_directory + '/' + mc_program):
		call(['mkdir', output_directory + '/' + mc_program])

	filename = str(uuid.uuid4()) + ".hepmc"
	
	if mc_program == "pythia":
		call( [ 'bin/pythia_weighted_events', temp_card_file, '{}/pythia/{}'.format(output_directory, filename) ] )
	elif mc_program == "herwig":
		call( [ 'Herwig', 'read', temp_card_file ] )
		call( [ 'Herwig', 'run', 'herwig_run.run', '-d1' ] )
		call( [ 'mv', "herwig_run.hepmc", '{}/herwig/{}'.format(output_directory, filename) ] )	# Move to correct location.
	elif mc_program == "sherpa":
		call( [ 'Sherpa', '-f', temp_card_file ] )
		call( [ 'mv', 'sherpa_long_MPI.hepmc', '{}/sherpa/{}'.format(output_directory, filename) ] )	# Move to correct location.
	else:
		raise Exception("Unknown Monte Carlo program enterred! I know only Pythia, Herwig and Sherpa.")

	return filename

def cleanup():
	files_to_remove = ['herwig_run.log', 'herwig_run.out', 'herwig_run.run', 'herwig_run.tex', 'MIG_P+P+_7000_NNPDF_2.db', 'MPI_Cross_Sections.dat', 'Results.db', 'Results.db.bak', 'Sherpa_References.tex']

	for f in files_to_remove:
		if os.path.exists(f):
			call(['rm', f])


def parse_hepmc(directory, filename, mc_header):
	call(['bin/parseHepMC', directory + '/' + filename, directory + '/' + filename[:-6] + '.mod', mc_header])


def remove_temp_file(temp_card_file):
	if os.path.exists(temp_card_file):
		call(['rm', temp_card_file])


def process_mc(mc_program, output_directory):

	if mc_program == "pythia":
		header = "Pythia_8215"
		card_file = "pythia_weighted_card.cmnd"
	elif mc_program == "herwig":
		header = "Herwig_7"
		card_file = "herwig_weighted_card.in"
	elif mc_program == "sherpa":
		raise Exception("Sherpa not implemented yet!")
		# header = "Sherpa_220"
		# card_file = "sherpa_card.dat"

	cleanup()
	
	number_of_files = int(math.ceil(total_number_of_events / events_per_file))
	source_card_file = cards_directory + "/" + card_file
	mc_header = header + '_Dijet{}'.format( int(get_pT_min(mc_program, source_card_file) ) )
	temp_card_file = './' + "temp_" + card_file

	print
	print "======================== MOD Monte Carlo ========================"
	print
	print "I've been asked to generate {} weighted events using {}.".format(total_number_of_events, mc_program.capitalize())
	print "I'm going to divide them up so that no file has more than {} events in it.".format(events_per_file)
	print "That means, there's going to be {} separate files in the directory: {}".format(number_of_files, output_directory)
	print "Monte Carlo header for the MOD file is going to be: {}".format(mc_header)
	print
	print "======================== MOD Monte Carlo ========================"
	print

	raw_input("Please press enter to continue!")

	for i in range(number_of_files):
		
		card_content = get_card( mc_program, source_card_file )
		
		write_card(card_content, temp_card_file)
		
		filename = run_mc_program(mc_program, output_directory, temp_card_file)
		
		cleanup()
		
		parse_hepmc(output_directory + "/" + mc_program, filename, mc_header)


		if remove_hepmc_files:
			if os.path.exists(output_directory + "/" + mc_program + "/" + filename):
				call( ['rm', output_directory + "/" + mc_program + "/" + filename] )

		remove_temp_file(temp_card_file)


	


start = time()

# process_mc("pythia", output_dir)

process_mc("herwig", output_dir)

# process_mc("sherpa", output_dir)

end = time()



print "Everything done in " + str(end - start) + " seconds!"