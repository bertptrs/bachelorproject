#include <getopt.h>
#include <cstdlib>
#include <cassert>

#include "Argstate.h"
#include "Graph.h"

Argstate::Argstate() :
	verbose(false),
	source(Graph::NO_NODE),
	sink(Graph::NO_NODE),
	help(false)
{
}

Argstate::Argstate(const Argstate& argstate) :
	verbose(argstate.verbose),
	filename(argstate.filename),
	source(argstate.source),
	sink(argstate.sink),
	help(argstate.help)
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
		{"output", required_argument, 0, 'o'},
		{"help", no_argument, &help, 1},
		{0, 0, 0, 0}
	};

	int opt;
	while ((opt = getopt_long(argc, argv, "vf:s:t:o:h", long_options, &optionIndex)) != -1) {
		switch(opt) {
			case 0:
				break;

			case '?':
				sprintf(optBuf, "Unknown parameter '%c'", optopt);
				showHelp(argc, argv);
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

			case 'v':
				verbose = true;
				break;

			case 'o':
				output = string(optarg);
				break;

			case 'h':
				help = true;
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

void Argstate::showHelp(int argc, char* const argv[], ostream& stream) const {
	assert(argc > 0);

	string program(argv[0]);
	stream << "Usage: " << program << " -f FILENAME" << endl << endl
		<< "Options:" << endl
		<< "\t-v/--verbose\tTurn verbosity on" << endl
		<< "\t-s/--source SOURCE source node, random if absent" << endl
		<< "\t-t/--sink SINK sink node, random if absent" << endl
		<< "\t-h/--help\tdisplay this message" << endl;
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

bool Argstate::isHelp() const {
	return (bool) help;
}

bool Argstate::shouldOutput() const {
	return output != "";
}

string Argstate::getOutputFilename() const {
	return output;
}
