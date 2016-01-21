from __future__ import division
from subprocess import call
import os
from time import time
import sys
from collections import defaultdict
import random
import uuid
import math

input_directory = sys.argv[1]
output_directory = sys.argv[2]


def get_info_about_mod_file(input_dir, f):

	filenames = f.split(".hepmc")
	name = filenames[0]

	corresponding_mod_file = name + ".mod"

	lines = open(input_dir + "/" + corresponding_mod_file, 'r').read().split("\n")

	for line in lines:
		words = line.split()
		if words[0] == "BeginEvent":
			version = words[2]
			truth_header = words[4]
			break

	truth_header_components = truth_header.split("_")

	mc_program_name = truth_header_components[0]
	mc_program_version = truth_header_components[1]
	data_details = truth_header_components[2]

	delphes_header = mc_program_name + "_" + mc_program_version + "_Delphes_332_" + data_details

	return (version, mc_program_name.lower(), delphes_header)


def run_delphes(input_dir, output_dir):
	
	hepmc_files = []

	for f in os.listdir(input_dir):
		if f.endswith(".hepmc"):
			hepmc_files.append(f)

	for f in hepmc_files:
		version, mc_program_name, delphes_header = get_info_about_mod_file(input_dir, f)

		print version, mc_program_name, delphes_header

		if not os.path.exists(output_dir + '/' + mc_program_name):
			call(['mkdir', output_dir + '/' + mc_program_name])

		call(['bin/runDelphes', 'cards/delphes_card_CMS_NoFastJet.tcl', input_dir + '/' + f, output_dir + '/' + mc_program_name + '/' + f.split('.hepmc')[0] + '.mod', version, delphes_header])
		




start = time()

run_delphes(input_directory, output_directory)

end = time()



print "Everything done in " + str(end - start) + " seconds!"