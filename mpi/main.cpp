#include <iostream>
#include <string>
#include "Argstate.h"
#include "mpi.h"
#include "PushLift.h"

using namespace std;

int main(int argc, char * argv[]) {
	MPI::Init(argc, argv);
	try {
		const Argstate args(argc, argv);
		PushLift algo(args);

		float flow = algo.flow();
		cout << "Max flow is " << flow << endl;

	} catch (ArgstateException ex) {
		cerr << ex.what() << endl;
		MPI::Finalize();
		return 2;
	}
	MPI::Finalize();
	return 0;
}
