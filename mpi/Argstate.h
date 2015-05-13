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


	public:
		Argstate();
		Argstate(const Argstate& argstate);
		
		bool isVerbose() const;
		bool isHelp() const;
		string getFilename() const;
		int getSource() const;
		int getSink() const;
		void showHelp(int, char* const[], ostream& stream = cout) const;

		void parseArgs(const int argc, char* const[]); 
};

class ArgstateException : public runtime_error {
	using runtime_error::runtime_error;
};

#endif
