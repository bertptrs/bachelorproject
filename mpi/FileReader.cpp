#include "FileReader.hpp"
#include <limits>

FileReader::FileReader(const string& filename) :
	filename(filename)
{
	input.open(filename.c_str());
}

FileReader::~FileReader() {
	input.close();
}

void FileReader::skipComments()
{
	while (!input.eof() && input.peek() == '%') {
		skipLine();
	}
}

void FileReader::skipLine()
{
	input.ignore(numeric_limits<streamsize>::max(), '\n');
}

Graph FileReader::read() {
	Graph graph;

	if (!input.good()) {
		throw FileReaderException("File could no be read");
	}

	// Matrix market reader borrowed from
	// http://www.cplusplus.com/forum/general/65804/

	// Ignore headers and comments
	skipComments();

	int M, N, L;

	input >> M >> N >> L;
	skipLine();

	for (int i = 0; i < L; i++) {
		skipComments();
		int m, n;
		weight_t c;
		input >> m >> n >> c;
		skipLine();

		graph.addEdge(m, n, c);
	}

	return graph;
}
