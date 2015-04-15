CXX=g++
CXXFLAGS=-Wall -Wextra -pedantic -O3 -std=c++11 -g
RM=rm -rf
MKDIR=mkdir -p

VPATH = src/\
		report/

OBJDIR=bin
DEPDIR=.deps

DIRS=$(OBJDIR)\
	 $(DEPDIR)\
	 report/build

CPPFILES=$(wildcard src/*.cpp)

_OBJS=$(patsubst %.cpp, %.o, $(notdir $(CPPFILES)))
DEPS=$(patsubst %.o, $(DEPDIR)/%.depend, $(_OBJS))
OBJS=$(patsubst %, $(OBJDIR)/%, $(_OBJS))

.PHONY: all clean run config report.pdf

all: test

report/build/thesis.pdf: thesis.tex $(shell find report -name "*.tex" -or -name "*.bib") | $(dir $@)
	latexmk -xelatex -cd $< -outdir=build -auxdir=build

$(DIRS):
	$(MKDIR) $@

clean:
	$(RM) test
	$(RM) report.pdf
	$(RM) $(DEPS)
	$(RM) $(DIRS)

test: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp | $(DIRS)
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(DEPDIR)/%.depend: %.cpp | $(DIRS)
	$(CXX) $(CXXFLAGS) -MM -MG -MT $(OBJDIR)/$(basename $(notdir $<)).o -MF $@ $<

include $(DEPS)
