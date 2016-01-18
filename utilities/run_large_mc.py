from __future__ import division
from subprocess import call
import os
from time import time
import sys
from collections import defaultdict
from random import randint
import uuid
import math

output_dir = sys.argv[1]
total_number_of_events = int(sys.argv[2])


cards_directory = "./cards"
events_per_file = 100

def get_number_of_events_per_file():
	if total_number_of_events < events_per_file:
		return total_number_of_events
	else:
		return events_per_file

def get_pT_min(mc_program, card_file):
	f = open(card_file, 'r')
  	lines = f.read().split("\n")

  	if mc_program == "pythia":
  		pT_min_line = [line for line in lines if 'pTHatMin' in line][0]
  		pT_min = float(pT_min_line.split("=")[1])

  	return pT_min

def get_pythia_card(f):
	
	random_seed = randint(0, 900000000)
	number_of_events = get_number_of_events_per_file()

	f = open(f, 'r')
  	lines = f.read().split("\n")
  	card_lines = filter(lambda line: "Main:numberOfEvents" not in line, lines) + [ "Random:seed = {}\nMain:numberOfEvents = {}".format(random_seed, number_of_events) ]

  	return "\n".join(map(str, card_lines))


def write_card(content, f):
	with open(f, 'w') as card_file:
		card_file.write(content)

def run_pythia(output_directory, temp_card_file):
	if not os.path.exists(output_directory + '/pythia'):
		call(['mkdir', output_directory + '/pythia'])

	filename = str(uuid.uuid4()) + ".hepmc"
	
	call( [ 'bin/generateEvents', temp_card_file, output_directory + '/pythia/{}'.format(filename) ] )

	return filename

def parse_hepmc(directory, filename, mc_header):
	call(['bin/parseHepMC', directory + '/' + filename, directory + '/' + filename[:-6] + '.mod', mc_header])


def pythia(output_directory):
	
	number_of_files = int(math.ceil(total_number_of_events / events_per_file))
	source_card_file = cards_directory + "/pythia_card.cmnd"
	pythia_header = 'Pythia_8215_Dijet{}'.format( int(get_pT_min('pythia', source_card_file) ) )
	temp_card_file = './tmp/pythia_card.cmnd'

	print
	print "I've been asked to generating {} events using Pythia.".format(total_number_of_events)
	print "I'm going to divide them up so that each one has just {} events in it.".format(events_per_file)
	print "That means, there's going to be {} separate files in the directory: {}".format(number_of_files, output_directory)
	print "Monte Carlo header for the MOD file is going to be: {}".format(pythia_header)
	print

	raw_input("Please press enter to continue!")

	for i in range(number_of_files):
		card_content = get_pythia_card( source_card_file )
		write_card(card_content, temp_card_file)
		filename = run_pythia(output_directory, temp_card_file)
		parse_hepmc(output_directory + "/pythia", filename, pythia_header)


start = time()

pythia(output_dir)

end = time()

# card_files = ['pythia_card.cmnd', 'herwig_card.in', 'sherpa_card.dat']


print "Everything done in " + str(end - start) + " seconds!"