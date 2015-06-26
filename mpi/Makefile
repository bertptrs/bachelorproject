CXXFLAGS=-O3 -Wall -Wextra -pedantic -std=c++11 -flto -march=native -g
CXX=mpic++

OBJECTS=main.o \
		Argstate.o \
		Graph.o \
		FileReader.o \
		MPICommunicator.o \
		PushLift.o

EXE=pushlift
TEST=mpitest

.PHONY: all clean

all: $(EXE) $(TEST)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(EXE)
	$(RM) $(TEST)

$(EXE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
