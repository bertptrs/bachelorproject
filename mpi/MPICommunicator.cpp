#include <cassert>

#include "MPICommunicator.hpp"

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

		COMM_WORLD.Send(&push, 1, pushTypeMPI, worker, CHANNEL_PUSHES);
		return true;
	} else {
		return false;
	}
}

void MPICommunicator::sendLift(int node, int delta, const set<int>& adjecentNodes) {
	LiftType lift(node, delta);
	for (int worker : adjecentNodes) {
		counter++;
		COMM_WORLD.Send(&lift, 1, liftTypeMPI, worker, CHANNEL_LIFTS);
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
	return getStreamWithPrefix(cerr);
}

ostream& MPICommunicator::getOutputStream() const {
	return getStreamWithPrefix(cout);
}

ostream& MPICommunicator::getStreamWithPrefix(ostream& stream) const
{
	return stream << "Worker " << rank << ": ";
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
}

void MPICommunicator::sendToken() {
	assert(hasToken);
	token.content.value += counter;

	if (isMaster()) {
		token.content.color = WHITE;
		token.content.value = 0;
	}

	COMM_WORLD.Send(token.data, 2, INT, nextWorker(), CHANNEL_TOKEN);

	hasToken = false;
	hasSent = true;
}

bool MPICommunicator::canShutdown() {
	while(!terminated) {
		if (hasToken) {
			if (isMaster() && hasSent) {
				if (color == WHITE && token.content.color == WHITE && token.content.value + counter == 0) {
					sendTermination();
					return true;
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
	vector<ReceivingObject> buffer(count);

	// Receive and queue the results
	COMM_WORLD.Recv(buffer.data(), count, datatype, status.Get_source(), channel);
	for (size_t i = 0; i < count; i++) {
		destination.push(buffer[i]);
	}
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

		COMM_WORLD.Send(&signal, 1, BOOL, i, CHANNEL_TERMINATION);
	}
	terminated = true;
}

void MPICommunicator::receieveTerminationMessage() {
	bool signal;

	COMM_WORLD.Recv(&signal, 1, BOOL, ANY_SOURCE, CHANNEL_TERMINATION);
	assert(signal && "Signal should be true.");
	terminated = true;
}
