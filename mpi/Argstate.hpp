#ifndef ARGSTATE_H
#define ARGSTATE_H

#include <string>
#include <stdexcept>
#include <iostream>

using namespace std;

class Argstate {
	private:
		int verbose;
		string filename;
		int source, sink;
		int help;
		string output;
		int implementation;

	public:
		Argstate();
		Argstate(const Argstate& argstate) = default;

		bool isVerbose() const;
		bool isHelp() const;
		string getFilename() const;
		int getSource() const;
		int getSink() const;
		string getOutputFilename() const;
		bool shouldOutput() const;
		void showHelp(int, char* const[], ostream& stream = cout) const;
		int getImplementation() const;

		void parseArgs(const int argc, char* const[]);
};

class ArgstateException : public runtime_error {
	using runtime_error::runtime_error;
};

#endif
