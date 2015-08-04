#include <iostream>
#include <fstream>
#include <string>
#include "Argstate.hpp"
#include "MPI.hpp"
#include "PushLiftImpl1.hpp"
#include "PushLiftImpl2.hpp"
#include "PushLiftImpl3.hpp"
#include "Timer.hpp"
#include <exception>

using namespace std;

void run(const Argstate& args) {
	PushLift* algo;
	switch (args.getImplementation()) {
		case 1:
			algo = new PushLiftImpl1(args);
			break;

		case 2:
			algo = new PushLiftImpl2(args);
			break;

		case 3:
			algo = new PushLiftImpl3(args);
			break;

		default:
			throw runtime_error("No such algorithm implementation.");
	}

	if (MPI::COMM_WORLD.Get_rank() == 0) {
		cout << "Algorithm initialized." << endl;
	}

	float flow = algo->flow();

	if (MPI::COMM_WORLD.Get_rank() == 0) {
		cout << "Max flow is " << flow << endl;

		if (args.shouldOutput()) {
			ofstream output(args.getOutputFilename().c_str());
			algo->writeFlow(output);
		}
	}

	delete algo;
}

template<typename Duration>
void printTimer(const string& message, const Duration& timerDuration)
{
	duration<double> actualDuration = timerDuration;
	cout << message << actualDuration.count() << "s" << endl;
}

int main(int argc, char * argv[]) {
	Argstate args;
	Timer<> timer;
	try {
		args.parseArgs(argc, argv);

		if (args.isHelp()) {
			args.showHelp(argc, argv);
			return 0;
		}
		Timer<> timer;
		MPI::Init(argc, argv);

		const int rank = MPI::COMM_WORLD.Get_rank();

		if (!rank) {
			printTimer("Initialization finshed after ", timer.get());
			timer.set();
		}

		run(args);

		if (!rank) {
			printTimer("Algorithm finished after ", timer.get());
			timer.set();
		}

		MPI::Finalize();

		if (!rank) {
			printTimer("Finalization finished after ", timer.get());
		}

		return 0;
	} catch (ArgstateException ex) {
		cerr << ex.what() << endl;

		args.showHelp(argc, argv, cerr);
		return 2;
	}

	return 1; // How did I get here?
}
