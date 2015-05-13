#include <iostream>
#include <string>
#include "Argstate.h"
#include "MPI.h"
#include "PushLift.h"

using namespace std;

int main(int argc, char * argv[]) {
	Argstate args;
	try {
		args.parseArgs(argc, argv);

		if (args.isHelp()) {
			args.showHelp(argc, argv);
			return 0;
		}

		MPI::Init(argc, argv);
		PushLift algo(args);

		float flow = algo.flow();
		cout << "Max flow is " << flow << endl;

		MPI::Finalize();

		return 0;
	} catch (ArgstateException ex) {
		cerr << ex.what() << endl;

		args.showHelp(argc, argv, cerr);
		return 2;
	}
	
	return 1; // How did I get here?
}
