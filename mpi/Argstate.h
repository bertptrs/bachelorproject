#ifndef ARGSTATE_H
#define ARGSTATE_H

#include <string>
#include <stdexcept>

using namespace std;

class Argstate {
	private:
		int verbose;
		string filename;
		int source, sink;

		void parseArgs(const int argc, char* const[]); 

	public:
		Argstate(int, char* const[]);
		Argstate(const Argstate& argstate);
		
		bool isVerbose() const;
		string getFilename() const;
		int getSource() const;
		int getSink() const;
};

class ArgstateException : public runtime_error {
	using runtime_error::runtime_error;
};

#endif
