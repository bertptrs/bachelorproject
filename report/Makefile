GRAPHS=graphs/graph_cage11_26345_36017.pdf\
	graphs/graph_implementations.pdf

DIRS=graphs

.PHONY: all clean

all: $(GRAPHS)
	latexmk -interaction=nonstopmode -xelatex thesis.tex

clean:
	latexmk -C
	$(RM) $(wildcard graphs/*)

graphs/graph_cage11_26345_36017.pdf: $(wildcard data/PUSHLIFT_IMPL1*) | graphs
	./generate-speedup-chart.py -o $@ $^

graphs/graph_implementations.pdf: $(wildcard data/PUSHLIFT_IMPL*) | graphs
	./generate-implementation-chart.py -o $@ $^

$(DIRS):
	mkdir $@
