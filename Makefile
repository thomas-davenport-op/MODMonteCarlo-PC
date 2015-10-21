PYTHIA_INC = $(PYTHIA8)/include
PYTHIA_LIB = $(PYTHIA8)/lib

HEPMC2_BIN=/home/aashish/hepmc/bin/
HEPMC2_INCLUDE=/home/aashish/hepmc/include
HEPMC2_LIB=/home/aashish/hepmc/lib


PATH_TO_FASTJET = /home/aashish/root/macros/fastjet-install/bin/fastjet-config
INCLUDE_FASTJET = /home/aashish/root/macros/fastjet-install/include

FASTINC = `$(PATH_TO_FASTJET) --cxxflags`
FASTLIB = `$(PATH_TO_FASTJET) --libs --plugins` -lRecursiveTools


CXX = g++
CXXFLAGS= -std=c++11





# all: examples/generateEvents.cc
# 	$(CXX) $(CXXFLAGS) -I$(PYTHIA_INC) -I$(HEPMC2_INCLUDE) examples/generateEvents.cc -o bin/generateEvents -lpythia8 -ldl -L$(PYTHIA_LIB) -lHepMC -L$(HEPMC2_LIB) -ldl


all: examples/parseHepMC.cc
	$(CXX) $(CXXFLAGS) -I$(HEPMC2_INCLUDE) examples/parseHepMC.cc -o bin/parseHepMC -lHepMC -L$(HEPMC2_LIB) -ldl -I$(INCLUDE_FASTJET) `$(PATH_TO_FASTJET) --libs --plugins` -lRecursiveTools



clean:
	rm bin/generateEvents