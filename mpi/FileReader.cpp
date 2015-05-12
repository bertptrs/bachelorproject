#include "FileReader.h"
#include <limits>

FileReader::FileReader(const string& filename) :
	filename(filename)
{
	input.open(filename.c_str());
}

FileReader::~FileReader() {
	input.close();
}

Graph FileReader::read() {
	Graph graph;

	if (!input.good()) {
		throw FileReaderException("File could no be read");
	}

	// Matrix market reader borrowed from
	// http://www.cplusplus.com/forum/general/65804/

	// Ignore headers and comments
	while (input.peek() == '%') {
		input.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	int M, N, L;

	input >> M >> N >> L;

	for (int i = 0; i < M; i++) {
		int m, n;
		float c;
		input >> m >> n >> c;

		graph.addEdge(m, n, c);
	}

	return graph;
}
