#ifndef MPICOMMUNICATOR_H
#define MPICOMMUNICATOR_H

#include <vector>
#include "Graph.hpp"
#include "MPI.hpp"
#include <iostream>
#include <queue>

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
		enum CHANNELS {
			CHANNEL_TOKEN,
			CHANNEL_PUSHES,
			CHANNEL_LIFTS,
			CHANNEL_TERMINATION
		};
		enum COLORS {
			WHITE,
			BLACK
		};

		const int rank;
		const int worldSize;
		MPI::Datatype pushTypeMPI;
		MPI::Datatype liftTypeMPI;
		// Used so that we can receive more than we put out.
		// This also means that we can receive even when there is no query to receive.
		queue<PushType> pushes;
		queue<LiftType> lifts;

		// Data for Safra's algorithm.
		int counter;
		int color;
		bool hasToken;
		bool hasSent;
		bool terminated;

		// Ring worker identifiers
		int nextWorker() const;
		int prevWorker() const;

		union {
			int data[2];
			struct {
				int color;
				int value;
			} content;
		} token;

		void sendPushes();
		void sendLifts();
		void sendTermination();

		// Actual communication methods
		void receiveMessages();
		void receiveToken();
		void receieveTerminationMessage();
		template<typename ReceivingObject>
			void receive(const int channel, const MPI::Datatype& dataType, queue<ReceivingObject>& destination);

		void sendToken();

		ostream& getStreamWithPrefix(ostream&) const;

	public:
		MPICommunicator();
		~MPICommunicator();
		int synchronize();

		int owner(int node) const;
		bool mine(int node) const;
		bool isMaster() const;

		bool sendPush(int from, int to, weight_t delta);
		void sendLift(int node, int delta, const set<int>& adjecentNodes);

		bool hasPush();
		PushType getPush();

		bool hasLift();
		LiftType getLift();

		bool canShutdown();

		ostream& getDebugStream() const;
		ostream& getOutputStream() const;

};

#endif
