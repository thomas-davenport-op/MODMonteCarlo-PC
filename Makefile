# PREFIX_LIB=../pythia8212/lib
# PREFIX_INCLUDE=../pythia8212/include

# -include Makefile.inc

# CXX_COMMON:= -I $(PREFIX_INCLUDE) $(CXX_COMMON) -Wl,-rpath $(PREFIX_LIB) -ldl
CXX_COMMON:= -I $(PREFIX_INCLUDE) -Wl,-rpath $(PREFIX_LIB) -ldl


CXX = g++
CXXFLAGS= -O3 -Wall -Woverloaded-virtual -g -std=c++11



FASTINC = `$(PREFIX_INCLUDE) --cxxflags`
FASTLIB = `$(PREFIX_INCLUDE) --libs --plugins` -lRecursiveTools



OBJDIR=src
EXECDIR=examples
BINDIR=bin
INCDIR=interface
INC= -I$(INCDIR)

_OBJ = 
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ:=.o))

_EXEC=generateEvents
EXEC=$(patsubst %,$(EXECDIR)/%,$(_EXEC:=.o))
BIN=$(patsubst %,$(BINDIR)/%,$(_EXEC))


# all:(PREFIX_LIB)/libpythia8.a $(CXX) $^ -o $@ $(CXX_COMMON)

# main% : main%.cc $(PREFIX_LIB)/libpythia8.a
# 	$(CXX) $^ -o $@ $(CXX_COMMON)


# $(PREFIX_LIB)/libpythia8.a $(CXX) -c -o $@ $< $(CXXFLAGS) $(CXX_COMMON)

# $(PREFIX_LIB)/libpythia8.a $(CXX) -c -o $@ $< $(CXXFLAGS) $(CXX_COMMON)

all: $(BIN)

$(OBJDIR)/%.o : $(OBJDIR)/%.cc
	$(PREFIX_LIB)/libpythia8.a $(CXX) -c -o $@ $< $(CXXFLAGS) $(INC) $(CXX_COMMON)

$(EXECDIR)/%.o : $(EXECDIR)/%.cc
	$(PREFIX_LIB)/libpythia8.a $(CXX) -c -o $@ $< $(CXXFLAGS) $(INC) $(CXX_COMMON)
	
$(BINDIR)/% : $(EXECDIR)/%.o $(OBJ)
	$(PREFIX_LIB)/libpythia8.a $(CXX) $< $(OBJ) -o $@ $(CXXFLAGS) $(CXX_COMMON)

.PHONY: clean
.PRECIOUS: $(OBJ) $(EXEC)

clean:
	rm $(OBJ) $(EXEC) $(BIN)