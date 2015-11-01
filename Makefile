PYTHIA_INC = $(PYTHIA8)/include
PYTHIA_LIB = $(PYTHIA8)/lib

HEPMC2_BIN=/home/aashish/hepmc/bin/
HEPMC2_INCLUDE=/home/aashish/hepmc/include
HEPMC2_LIB=/home/aashish/hepmc/lib

DELPHES_INC = $(DELPHES)
DELPHES_LIB = $(DELPHES)


PATH_TO_FASTJET = /usr/local/bin/fastjet-config
INCLUDE_FASTJET = /usr/local/include

FASTINC = `$(PATH_TO_FASTJET) --cxxflags`
FASTLIB = `$(PATH_TO_FASTJET) --libs --plugins` -lRecursiveTools


CXX = g++
CXXFLAGS= -std=c++11





# all: examples/generateEvents.cc
# 	$(CXX) $(CXXFLAGS) -I$(PYTHIA_INC) -I$(HEPMC2_INCLUDE) examples/generateEvents.cc -o bin/generateEvents -lpythia8 -ldl -L$(PYTHIA_LIB) -lHepMC -L$(HEPMC2_LIB) -ldl


all: examples/parseHepMC.cc
	$(CXX) $(CXXFLAGS) -I$(HEPMC2_INCLUDE) examples/parseHepMC.cc -o bin/parseHepMC -lHepMC -L$(HEPMC2_LIB) -ldl -I$(INCLUDE_FASTJET) `$(PATH_TO_FASTJET) --libs --plugins` -lRecursiveTools



# all: examples/parseDelphesOutput.cc
# 	$(CXX) $(CXXFLAGS) -I$(HEPMC2_INCLUDE) examples/parseDelphesOutput.cc -o bin/parseDelphesOutput -lDelphes -L$(DELPHES_LIB) -L$(DELPHES_INC)/external -ldl -I$(DELPHES_INC) -I$(DELPHES_INC)/external -I$(INCLUDE_FASTJET) `$(PATH_TO_FASTJET) --libs --plugins` `root-config --cflags --glibs`


clean:
	rm bin/generateEvents