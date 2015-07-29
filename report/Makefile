BUILDDIR=build

GRAPHS=graphs/graph_cage11_26345_36017.pdf

.PHONY: all clean

all: $(GRAPHS)
	latexmk -xelatex thesis.tex -outdir=$(BUILDDIR) -auxdir=$(BUILDDIR)

clean:
	$(RM) $(wildcard $(BUILDDIR)/*)
	$(RM) $(wildcard graphs/*)

graphs/graph_cage11_26345_36017.pdf: $(wildcard data/PUSHLIFT_26345_36017_*)
	./generate-speedup-chart.py -o $@ $^
