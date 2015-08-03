#include <iostream>
#include "MPI.hpp"

using namespace std;

// This program attempts to test the current environment for MPI.
// It calculates sum for the arithmetic sequence 1..WORLD_SIZE using a ring
// of MPI nodes. The it calculates the result again using the direct formula.
//
// It prints those two numbers. If they differ, things are weird.

constexpr int sum(int begin, int end, int step = 1) {
	return (((end - begin)/step + 1) * (end + begin)) / 2;
}

int main(int argc, char* argv[]) {
	MPI::Init(argc, argv);
	const int rank = MPI::COMM_WORLD.Get_rank();
	const int worldSize = MPI::COMM_WORLD.Get_size();

	if (rank == 0) {
		cout << "Calculating sum of 1.." << worldSize << " using MPI" << endl;
		int val = 1;
		MPI::COMM_WORLD.Send(&val, 1, MPI::INT, (rank + 1) % worldSize, 0);
		MPI::COMM_WORLD.Recv(&val, 1, MPI::INT, (rank + worldSize - 1) % worldSize, 0);
		cout << "Value is " << val << endl;
		cout << "Correct is " << sum(1, worldSize) << endl;
	} else {
		int val;
		MPI::COMM_WORLD.Recv(&val, 1, MPI::INT, (rank + worldSize - 1) % worldSize, 0);
		val += rank + 1;
		MPI::COMM_WORLD.Send(&val, 1, MPI::INT, (rank + 1) % worldSize, 0);
	}

	MPI::Finalize();
	return 0;
}
