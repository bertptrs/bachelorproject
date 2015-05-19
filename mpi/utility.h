#ifndef UTILITY_H
#define UTILITY_H

#include <utility>

using namespace std;

template<class T1, class T2>
auto reverse(pair<T1, T2> p) -> pair<T2, T1> {
	return make_pair(p.second, p.first);
}

#endif
