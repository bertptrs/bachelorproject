#ifndef EKTUPLE
#define EKTUPLE

#include <queue>
#include "FFTuple.h"

/**
 * Tuple based implementation of the Edmonds-Karp algorithm.
 *
 * This is basically Ford-Fulkerson with a breadth first search.
 */
class EKTuple : public FFTuple {
	private:
		template<class Predicate>
			void addTodo(queue<int>& todo, Predicate p) const {
				for (int entry : getEdges(p)) {
					todo.push(entry);
				}
			}
	protected:
		virtual vector<int> getPath(const vector<int> curPath, const int source, const int dest) const;
};

#endif
