CXX=g++
CXXFLAGS=-Wall -Wextra -pedantic -O3 -std=c++11
RM=rm -rf
MKDIR=mkdir -p

VPATH = src/

OBJDIR=bin
DEPDIR=.deps

DIRS=$(OBJDIR)\
	 $(DEPDIR)

CPPFILES=$(wildcard src/*.cpp)

_OBJS=$(patsubst %.cpp, %.o, $(notdir $(CPPFILES)))
DEPS=$(patsubst %.o, $(DEPDIR)/%.depend, $(_OBJS))
OBJS=$(patsubst %, $(OBJDIR)/%, $(_OBJS))

.PHONY: all clean run config

all: test

$(DIRS):
	$(MKDIR) $@

clean:
	$(RM) test
	$(RM) $(DEPS)
	$(RM) $(DIRS)

test: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp | $(DIRS)
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(DEPDIR)/%.depend: %.cpp | $(DIRS)
	$(CXX) $(CXXFLAGS) -MM -MG -MT $(OBJDIR)/$(basename $(notdir $<)).o -MF $@ $<

include $(DEPS)
