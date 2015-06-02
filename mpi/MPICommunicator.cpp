#include <cassert>

#include "MPICommunicator.h"
#include "channels.h"

using namespace MPI;

PushType::PushType(int from, int to, weight_t delta) :
	from(from),
	to(to),
	amount(delta)
{
}

LiftType::LiftType(int node, int delta) :
	node(node),
	delta(delta)
{
}

MPICommunicator::MPICommunicator() :
	rank(COMM_WORLD.Get_rank()),
	worldSize(COMM_WORLD.Get_size())
{
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

bool MPICommunicator::sendPush(int from, int to, weight_t delta) {
	if (!mine(to)) {
		PushType push(from, to, delta);
		int worker = owner(to);

		COMM_WORLD.Isend(&push, 1, pushTypeMPI, worker, CHANNEL_PUSHES);
		return true;
	} else {
		return false;
	}
}

void MPICommunicator::sendLift(int node, int delta, const set<int>& adjecentNodes) {
	LiftType lift(node, delta);
	for (int worker : adjecentNodes) {
		COMM_WORLD.Isend(&lift, 1, liftTypeMPI, worker, CHANNEL_LIFTS);
	}
}

bool MPICommunicator::hasPush() const {
	return COMM_WORLD.Iprobe(ANY_SOURCE, CHANNEL_PUSHES);
}

PushType MPICommunicator::getPush() {
	assert(hasPush() && "Should have a push available");
	
	PushType push;
	COMM_WORLD.Recv(&push, 1, pushTypeMPI, ANY_SOURCE, CHANNEL_PUSHES);

	return push;
}

bool MPICommunicator::hasLift() const {
	return COMM_WORLD.Iprobe(ANY_SOURCE, CHANNEL_LIFTS);
}

LiftType MPICommunicator::getLift() {
	assert(hasLift() && "Should have lift available");

	LiftType lift;
	COMM_WORLD.Recv(&lift, 1, liftTypeMPI, ANY_SOURCE, CHANNEL_LIFTS);

	return lift;
}
