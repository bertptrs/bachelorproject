BUILDDIR=build

GRAPHS=graphs/graph_cage11_26345_36017.pdf

DIRS=graphs

.PHONY: all clean

all: $(GRAPHS)
	latexmk -interaction=nonstopmode -xelatex thesis.tex -outdir=$(BUILDDIR) -auxdir=$(BUILDDIR)

clean:
	$(RM) $(wildcard $(BUILDDIR)/*)
	$(RM) $(wildcard graphs/*)

graphs/graph_cage11_26345_36017.pdf: $(wildcard data/PUSHLIFT_26345_36017_*) | graphs
	./generate-speedup-chart.py -o $@ $^

$(DIRS):
	mkdir $@
