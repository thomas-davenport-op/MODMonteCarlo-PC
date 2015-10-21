PYTHIA_INC = $(PYTHIA8)/include
PYTHIA_LIB = $(PYTHIA8)/lib

HEPMC2_BIN=/home/aashish/hepmc/bin/
HEPMC2_INCLUDE=/home/aashish/hepmc/include
HEPMC2_LIB=/home/aashish/hepmc/lib


CXX = g++
# CXXFLAGS= -Wall -Woverloaded-virtual -g -std=c++11
CXXFLAGS= -std=c++11

# $(CXX) $^ -o $@ -I$(HEPMC2_INCLUDE) $(CXX_COMMON)\
	 # -L$(HEPMC2_LIB) -Wl,-rpath $(HEPMC2_LIB) -lHepMC




# all: examples/generateEvents.cc
# 	$(CXX) $(CXXFLAGS) -I$(PYTHIA_INC) -I$(HEPMC2_INCLUDE) examples/generateEvents.cc -o bin/generateEvents -lpythia8 -ldl -L$(PYTHIA_LIB) -lHepMC -L$(HEPMC2_LIB) -ldl


all: examples/parseHepMC.cc
	$(CXX) $(CXXFLAGS) -I$(HEPMC2_INCLUDE) examples/parseHepMC.cc -o bin/parseHepMC -lHepMC -L$(HEPMC2_LIB) -ldl



clean:
	rm bin/generateEvents