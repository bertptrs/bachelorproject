#include <cassert>
#include "ActiveState.h"
#include "MPI.h"
#include "channels.h"

using namespace MPI;

const int ActiveState::NO_PARENT = -1;

ActiveState::ActiveState(int master) :
	rank(COMM_WORLD.Get_rank()),
	worldSize(COMM_WORLD.Get_size()),
	master(master),
	parent(NO_PARENT)
{
	if (isMaster()) {
		parent = 0; // Master is its own parent
	}
}

void ActiveState::activate(const int& newParent) {
	if (parent == NO_PARENT) {
		parent = newParent;
	} else {
		int message;
		message = REMOVE_CHILD;
		COMM_WORLD.Isend(&message, 1, INT, parent, CHANNEL_ACTIVESTATE);
	}
}

bool ActiveState::attemptShutdown() {
	checkMessages();
	if (children.empty()) {
		deactivate();

		if (isMaster()) {
			sendShutdownCommand();
			return true;
		} else {
			return getShutdownCommand();
		}

	} else {
		return false;
	}
}

void ActiveState::deactivate() {
	if (!isMaster()) {
		assert(parent != NO_PARENT && "Should have a parent before deactivating.");

		int message = REMOVE_CHILD;

		COMM_WORLD.Isend(&message, 1, INT, parent, CHANNEL_ACTIVESTATE);
		parent = NO_PARENT;
	}
}

void ActiveState::getChild(const int& child) {
	assert(child != rank && "Should not attempt to register self as child");

	checkMessages();

	if (!children.count(child)) {
		children.insert(child);
		int message = PARENT_REQUEST;

		COMM_WORLD.Isend(&message, 1, INT, child, CHANNEL_ACTIVESTATE);
	}
}

void ActiveState::checkMessages() {
	Status status;
	while (COMM_WORLD.Iprobe(ANY_SOURCE, CHANNEL_ACTIVESTATE, status)) {
		assert(status.Get_count(INT) == 1 && "One message at a time");

		int message;
		COMM_WORLD.Recv(&message, 1, INT, ANY_SOURCE, CHANNEL_ACTIVESTATE, status);

		switch ((ActiveStateMessage) message) {
			case REMOVE_CHILD:
				children.erase(status.Get_source());
				break;

			case PARENT_REQUEST:
				activate(status.Get_source());
				break;

			case SHUTDOWN:
				assert(false && "Received premature shutdown request!");
				break;

			default:
				cerr << "Received unparseable status message " << message << endl;
				assert(false && "Performing hard shutdown.");
				break;
		}
	}
}

bool ActiveState::isMaster() {
	return rank == master;
}

bool ActiveState::getShutdownCommand() {
	Status status;
	int message;
	COMM_WORLD.Recv(&message, 1, INT, ANY_SOURCE, CHANNEL_ACTIVESTATE, status);
	switch ((ActiveStateMessage) message) {
		case PARENT_REQUEST:
			activate(status.Get_source());
			return false;

		case SHUTDOWN:
			return true;

		default:
			assert(false && "Invalid message received");
			return false;
	}
}

void ActiveState::sendShutdownCommand() {
	assert(isMaster() && "Only master should issue shutdown command");

	const int message = SHUTDOWN;

	for (int i = 0; i < worldSize; i++) {
		if (i == master) {
			continue;
		}

		COMM_WORLD.Isend(&message, 1, INT, i, CHANNEL_ACTIVESTATE);
	}
}
