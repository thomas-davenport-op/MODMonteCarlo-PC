PYTHIA_INC = $(PYTHIA8)/include
PYTHIA_LIB = $(PYTHIA8)/lib


CXX = g++
# CXXFLAGS= -Wall -Woverloaded-virtual -g -std=c++11
CXXFLAGS= -std=c++11


all: examples/generateEvents.cc
	$(CXX) $(CXXFLAGS) -I$(PYTHIA_INC) examples/generateEvents.cc -o bin/generateEvents -L$(PYTHIA_LIB) -lpythia8 -ldl


clean:
	rm bin/generateEvents