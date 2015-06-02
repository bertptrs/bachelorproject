CXXFLAGS=-O3 -Wall -Wextra -pedantic -std=c++11 -flto -march=native -g
CXX=mpic++

OBJECTS=main.o \
		Argstate.o \
		Graph.o \
		FileReader.o \
		MPICommunicator.o \
		PushLift.o

EXE=pushlift

.PHONY: all clean test

all: $(EXE)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
