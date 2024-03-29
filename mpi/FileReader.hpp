#ifndef FILEREADER_H
#define FILEREADER_H

#include <exception>
#include "Graph.hpp"
#include <fstream>

using namespace std;

class FileReaderException : public runtime_error {
	using runtime_error::runtime_error;
};

class FileReader {
	private:
		ifstream input;
		string filename;

		inline void skipComments();
		inline void skipLine();

	public:
		FileReader(const string&);
		~FileReader();

		Graph read();
};


#endif
