PYTHIA_INC = /home/cms-opendata/pythia8244/include
PYTHIA_LIB = /home/cms-opendata/pythia8244/lib

HEPMC2_BIN=/usr/local/bin
HEPMC2_INCLUDE=/usr/local/include
HEPMC2_LIB=/usr/local/lib

DELPHES_INC = $(DELPHES)
DELPHES_LIB = $(DELPHES)


PATH_TO_FASTJET = /home/cms-opendata/fastjet-install/bin/fastjet-config
INCLUDE_FASTJET = /home/cms-opendata/fastjet-install/include

FASTINC = `$(PATH_TO_FASTJET) --cxxflags`
FASTLIB = `$(PATH_TO_FASTJET) --libs --plugins` -lRecursiveTools


CXX = g++
CXXFLAGS= -std=c++11





# all: src/generateEvents.cc
# 	$(CXX) $(CXXFLAGS) -I$(PYTHIA_INC) -I$(HEPMC2_INCLUDE) src/generateEvents.cc -o bin/generateEvents -lpythia8 -ldl -L$(PYTHIA_LIB) -lHepMC -L$(HEPMC2_LIB) -ldl -I$(INCLUDE_FASTJET) `$(PATH_TO_FASTJET)  --libs --plugins` -lRecursiveTools


all: src/pythia_weighted_events.cc src/parseHepMC.cc
	$(CXX) $(CXXFLAGS) -I$(PYTHIA_INC) -I$(HEPMC2_INCLUDE) src/pythia_weighted_events.cc -o bin/pythia_weighted_events -lpythia8 -ldl -L$(PYTHIA_LIB) -lHepMC -L$(HEPMC2_LIB) -ldl -I$(INCLUDE_FASTJET) `$(PATH_TO_FASTJET)  --libs --plugins` -lRecursiveTools
	$(CXX) $(CXXFLAGS) -I$(HEPMC2_INCLUDE) src/parseHepMC.cc -o bin/parseHepMC -lHepMC -L$(HEPMC2_LIB) -ldl -I$(INCLUDE_FASTJET) `$(PATH_TO_FASTJET) --libs --plugins` -lRecursiveTools



#all: src/parseHepMC.cc
#	$(CXX) $(CXXFLAGS) -I$(HEPMC2_INCLUDE) src/parseHepMC.cc -o bin/parseHepMC -lHepMC -L$(HEPMC2_LIB) -ldl -I$(INCLUDE_FASTJET) `$(PATH_TO_FASTJET) --libs --plugins` -lRecursiveTools




# all: src/runDelphes.cc
# 	$(CXX) $(CXXFLAGS) -I$(HEPMC2_INCLUDE) src/runDelphes.cc -o bin/runDelphes -lHepMC -lDelphes -L$(DELPHES_LIB) -L$(HEPMC2_LIB) -L$(DELPHES_INC)/external -ldl -I$(DELPHES_INC) -I$(DELPHES_INC)/external -I$(INCLUDE_FASTJET) `$(PATH_TO_FASTJET) --libs --plugins` `root-config --cflags --glibs`


clean:
	rm bin/generateEvents bin/pythia_weighted_events
