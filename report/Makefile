GRAPHS=graphs/graph_speedup.pdf \
	graphs/graph_implementations.pdf

DIRS=graphs

.PHONY: all clean

all: $(GRAPHS)
	latexmk -interaction=nonstopmode -xelatex thesis.tex

clean:
	latexmk -C
	$(RM) $(wildcard graphs/*)

graphs/graph_speedup.pdf: $(wildcard data/*.tar.gz) | graphs
	./generate-speedup-chart.py -o $@ $^

graphs/graph_implementations.pdf: $(wildcard data/cage11-impl*.tar.gz) | graphs
	./generate-implementation-chart.py -o $@ $^

$(DIRS):
	mkdir $@
