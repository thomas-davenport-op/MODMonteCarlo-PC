from __future__ import division
from subprocess import call
import os
from time import time
import sys

input_directory = sys.argv[1]
output_directory = sys.argv[2]


def parse_hepmc(input_directory, output_directory, mc_header):
	for f in os.listdir(input_directory):
	    if f.endswith(".hepmc"): 
	        call(['bin/parseHepMC', input_directory + '/' + f, output_directory + '/' + f[:-6] + '.mod', mc_header])

start = time()

mc_headers = ['Pythia_8215_Dijet100']
parse_hepmc(input_directory, output_directory)

end = time()




print "Everything done in " + str(end - start) + " seconds!"