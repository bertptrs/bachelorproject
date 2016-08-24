GRAPHS=graphs/graph_speedup.pdf \
	   graphs/graph_implementations.pdf \
	   graphs/graph_initialization.pdf

DIRS=graphs generated

.PHONY: all clean

all: $(GRAPHS) generated/runtime-table.tex
	latexmk -interaction=nonstopmode -xelatex thesis.tex

clean:
	latexmk -C
	$(RM) $(wildcard graphs/*)
	$(RM) $(wildcard generated/*)

graphs/graph_speedup.pdf: $(sort $(wildcard data/*.tar.gz)) | graphs
	./generate-speedup-chart.py -o $@ $^

graphs/graph_implementations.pdf: $(wildcard data/internet-impl*.tar.gz) | graphs
	./generate-implementation-chart.py -o $@ $^

graphs/graph_initialization.pdf: $(wildcard data/*.tar.gz) | graphs
	./generate-initialization-chart.py -o $@ $^

generated/runtime-table.tex: $(wildcard data/*.tar.gz) | generated
	./generate-runtime-table.py -o $@ -c 1 4 8 16 64 120 -- $^

$(DIRS):
	mkdir $@
