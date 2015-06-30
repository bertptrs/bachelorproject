#include <cassert>

#include "MPICommunicator.h"

using namespace MPI;

PushType::PushType(int from, int to, weight_t delta) :
	from(from),
	to(to),
	amount(delta)
{
}

ostream& operator<<(ostream& os, PushType& push)
{
	return os << "Push(" << push.from << ", " << push.to << ", " << push.amount << ")";
}

LiftType::LiftType(int node, int delta) :
	node(node),
	delta(delta)
{
}

ostream& operator<<(ostream& os, LiftType& lift) {
	return os << "Lift(" << lift.node << ", " << lift.delta << ")";
}

MPICommunicator::MPICommunicator() :
	rank(COMM_WORLD.Get_rank()),
	worldSize(COMM_WORLD.Get_size()),
	counter(0),
	color(WHITE),
	hasSent(false),
	terminated(false)
{
	// Master has token initially
	hasToken = isMaster();
	// Allocate MPI datatypes
	Aint extent, lb;
	INT.Get_extent(lb, extent);
	const int blockLengths[] = {2, 1};
	const Aint offsets[] = {0, 2 * extent};
	const Datatype types[] = {INT, DOUBLE_PRECISION};
	pushTypeMPI = Datatype::Create_struct(2, blockLengths, offsets, types);
	pushTypeMPI.Commit();
	liftTypeMPI = INT.Create_contiguous(2);
	liftTypeMPI.Commit();
}

MPICommunicator::~MPICommunicator() {
	// Deallocate MPI datatypes
	liftTypeMPI.Free();
	pushTypeMPI.Free();
}

int MPICommunicator::owner(int node) const {
	return node % worldSize;
}

bool MPICommunicator::mine(int node) const {
	return owner(node) == rank;
}

bool MPICommunicator::isMaster() const {
	return rank == 0;
}

bool MPICommunicator::sendPush(int from, int to, weight_t delta) {
	if (!mine(to)) {
		PushType push(from, to, delta);
		int worker = owner(to);
		counter++;

		COMM_WORLD.Isend(&push, 1, pushTypeMPI, worker, CHANNEL_PUSHES);
		getDebugStream() << "Sending " << push << endl;
		return true;
	} else {
		return false;
	}
}

void MPICommunicator::sendLift(int node, int delta, const set<int>& adjecentNodes) {
	LiftType lift(node, delta);
	getDebugStream() << "Sending " << lift << endl;
	for (int worker : adjecentNodes) {
		counter++;
		COMM_WORLD.Isend(&lift, 1, liftTypeMPI, worker, CHANNEL_LIFTS);
	}
}

bool MPICommunicator::hasPush() {
	receiveMessages();
	return !pushes.empty();
}

PushType MPICommunicator::getPush() {
	assert(hasPush() && "Should have a push available");

	PushType push = pushes.front();
	pushes.pop();

	return push;
}

bool MPICommunicator::hasLift() {
	receiveMessages();
	return !lifts.empty();
}

LiftType MPICommunicator::getLift() {
	assert(hasLift() && "Should have lift available");

	LiftType lift = lifts.front();
	lifts.pop();

	return lift;
}

ostream& MPICommunicator::getDebugStream() const {
	return cerr << "Worker " << rank << ": ";
}

void MPICommunicator::receiveMessages() {
	Status status;
	while (COMM_WORLD.Iprobe(ANY_SOURCE, ANY_TAG, status)) {
		switch (status.Get_tag()) {
			case CHANNEL_PUSHES:
				receive<PushType>(CHANNEL_PUSHES, pushTypeMPI, pushes);
				break;

			case CHANNEL_LIFTS:
				receive<LiftType>(CHANNEL_LIFTS, liftTypeMPI, lifts);
				break;

			case CHANNEL_TOKEN:
				assert(status.Get_source() == prevWorker());
				receiveToken();
				break;

			case CHANNEL_TERMINATION:
				receieveTerminationMessage();
				break;
		}
	}
}

void MPICommunicator::receiveToken() {
	COMM_WORLD.Recv(token.data, 2, INT, prevWorker(), CHANNEL_TOKEN);

	if (color == BLACK)  {
		token.content.color = BLACK;
		color = WHITE;
	}

	hasToken = true;
	getDebugStream() << "Receieved token of color "
		<< (token.content.color == WHITE ? "white" : "black")
		<< " and value " << token.content.value << endl;
}

void MPICommunicator::sendToken() {
	assert(hasToken);
	token.content.value += counter;

	if (isMaster()) {
		token.content.color = WHITE;
		token.content.value = 0;
	}

	COMM_WORLD.Isend(token.data, 2, INT, nextWorker(), CHANNEL_TOKEN);

	hasToken = false;
	hasSent = true;
	getDebugStream() << "Sent token" << endl;
}

bool MPICommunicator::canShutdown() {
	getDebugStream() << "Attempting to shut down." << endl;
	while(!terminated) {
		if (hasToken) {
			if (isMaster() && hasSent) {
				if (color == WHITE && token.content.color == WHITE && token.content.value + counter == 0) {
					getDebugStream() << "Noticed I should shut down." << endl;
					sendTermination();
					return true;
				} else {
					getDebugStream() << "Could not terminate yet." << endl;
				}
			}
			sendToken();
		}

		if (!pushes.empty() || !lifts.empty()) {
			return false;
		}

		// Stall until a new message is available
		COMM_WORLD.Probe(ANY_SOURCE, ANY_TAG);
		receiveMessages();
	}

	return true;

}

template<typename ReceivingObject>
void MPICommunicator::receive(const int channel, const Datatype& datatype, queue<ReceivingObject>& destination) {
	Status status;
	COMM_WORLD.Iprobe(ANY_SOURCE, channel, status);
	size_t count = status.Get_count(datatype);

	if (!count) return;

	counter -= count;
	color = BLACK;

	// Allocate a sufficient buffer for the results.
	ReceivingObject* buffer = new ReceivingObject[count];

	// Receive and queue the results
	COMM_WORLD.Recv(buffer, count, datatype, status.Get_source(), channel);
	for (size_t i = 0; i < count; i++) {
		destination.push(buffer[i]);
		getDebugStream() << "Received " << buffer[i] << endl;
	}

	delete buffer; // Dispose of the buffer.
}

int MPICommunicator::nextWorker() const {
	return (rank + 1) % worldSize;
}

int MPICommunicator::prevWorker() const {
	return (rank + worldSize - 1) % worldSize;
}

void MPICommunicator::sendTermination() {
	bool signal = true;
	for (int i = 0; i < worldSize; i++) {
		if (i == rank) {
			continue;
		}

		COMM_WORLD.Isend(&signal, 1, BOOL, i, CHANNEL_TERMINATION);
	}
	terminated = true;
}

void MPICommunicator::receieveTerminationMessage() {
	bool signal;

	COMM_WORLD.Recv(&signal, 1, BOOL, ANY_SOURCE, CHANNEL_TERMINATION);
	assert(signal && "Signal should be true.");
	terminated = true;
}
