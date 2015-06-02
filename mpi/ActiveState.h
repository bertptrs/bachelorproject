#ifndef ACTIVESTATE_H
#define ACTIVESTATE_H

#include <set>

using namespace std;

class ActiveState {
	private:
		enum ActiveStateMessage {
			REMOVE_CHILD = 2,
			PARENT_REQUEST = 3,
			SHUTDOWN = 4
		};
		static const int NO_PARENT;

		const int rank;
		const int worldSize;
		const int master;

		int parent;
		set<int> children;
		void activate(const int&);
		void deactivate();
		void checkMessages();
		
		bool getShutdownCommand();
		void sendShutdownCommand();

		bool isMaster();

	public:
		ActiveState(int master = 0);

		bool attemptShutdown();

		void getChild(const int& child);
};

#endif
