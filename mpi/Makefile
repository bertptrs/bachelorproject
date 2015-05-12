CXXFLAGS=-O3 -Wall -Wextra -pedantic -std=c++11 -flto -Wno-literal-suffix -march=native -g
CXX=mpic++

OBJECTS=main.o \
		Argstate.o \
		Graph.o \
		FileReader.o \
		PushLift.o

EXE=pushlift

.PHONY: all clean test

all: $(EXE)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
