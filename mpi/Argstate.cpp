#include "Argstate.h"
#include <getopt.h>
#include <cstdlib>
#include "Graph.h"

Argstate::Argstate(const int argc, char * const argv[]) :
	verbose(false),
	source(Graph::NO_NODE),
	sink(Graph::NO_NODE)
{
	parseArgs(argc, argv);
}

Argstate::Argstate(const Argstate& argstate) :
	verbose(argstate.verbose),
	filename(argstate.filename),
	source(argstate.source),
	sink(argstate.sink)
{
}

void Argstate::parseArgs(const int argc, char * const argv[]) {
	bool hasFile = false;
	int optionIndex = 0;
	char optBuf[100];

	static struct option long_options[] = {
		{"verbose", no_argument, &verbose, 1},
		{"filename", required_argument, 0, 'f'},
		{"source", required_argument, 0, 's'},
		{"sink", required_argument, 0, 't'},
		{0, 0, 0, 0}
	};

	int opt;
	while ((opt = getopt_long(argc, argv, "vf:s:t:", long_options, &optionIndex)) != -1) {
		switch(opt) {
			case 0:
				break;

			case '?':
				sprintf(optBuf, "Unknown parameter '%c'", optopt);
				throw ArgstateException(string(optBuf));

			case 'f':
				filename = string(optarg);
				hasFile = true;
				break;

			case 's':
				source = atoi(optarg);
				break;

			case 't':
				sink = atoi(optarg);
				break;
		}
	}

	if (!hasFile) {
		throw ArgstateException("No input filename provided.");
	}
}

bool Argstate::isVerbose() const {
	return (bool) verbose;
}

string Argstate::getFilename() const {
	return filename;
}

int Argstate::getSource() const {
	return source;
}

int Argstate::getSink() const {
	return sink;
}
