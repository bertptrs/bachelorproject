#include <iostream>
#include <fstream>
#include <string>
#include "Argstate.h"
#include "MPI.h"
#include "PushLift.h"

using namespace std;

void run(const Argstate& args) {
	PushLift algo(args);

	float flow = algo.flow();

	if (MPI::COMM_WORLD.Get_rank() == 0) {
		cout << "Max flow is " << flow << endl;

		if (args.shouldOutput()) {
			ofstream output(args.getOutputFilename().c_str());
			algo.writeFlow(output);
		}
	}
}

int main(int argc, char * argv[]) {
	Argstate args;
	try {
		args.parseArgs(argc, argv);

		if (args.isHelp()) {
			args.showHelp(argc, argv);
			return 0;
		}

		MPI::Init(argc, argv);

		run(args);

		MPI::Finalize();

		return 0;
	} catch (ArgstateException ex) {
		cerr << ex.what() << endl;

		args.showHelp(argc, argv, cerr);
		return 2;
	}
	
	return 1; // How did I get here?
}
