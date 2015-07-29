BUILDDIR=build

.PHONY: all clean

all:
	latexmk -xelatex thesis.tex -outdir=$(BUILDDIR) -auxdir=$(BUILDDIR)

clean:
	$(RM) $(wildcard $(BUILDDIR)/*)
