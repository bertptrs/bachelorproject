#ifndef MPICOMMUNICATOR_H
#define MPICOMMUNICATOR_H

#include <vector>
#include "Graph.h"
#include "MPI.h"
#include <iostream>

using namespace std;

struct PushType {
	int from, to;
	weight_t amount;

	PushType(int from = 0, int to = 0, weight_t delta = 0);
	PushType(const PushType& push) = default;
};

ostream& operator<<(ostream& os, const PushType& push);

struct LiftType {
	int node;
	int delta;

	LiftType(int node = 0, int delta = 0);
	LiftType(const LiftType& lift) = default;
};

ostream& operator<<(ostream& os, const LiftType& lift);

class MPICommunicator {
	private:
		const int rank;
		const int worldSize;
		MPI::Datatype pushTypeMPI;
		MPI::Datatype liftTypeMPI;

		void sendPushes();
		void sendLifts();

	public:
		MPICommunicator();
		~MPICommunicator();
		int synchronize();

		int owner(int node) const;
		bool mine(int node) const;
		bool isMaster() const;

		bool sendPush(int from, int to, weight_t delta);
		void sendLift(int node, int delta, const set<int>& adjecentNodes);

		bool hasPush() const;
		PushType getPush();
		
		bool hasLift() const;
		LiftType getLift();

		ostream& getDebugStream() const;

};

#endif
