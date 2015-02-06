#include <algorithm>
#include <cassert>
#include <cstdlib>

using namespace std;

// Check if a range contains a value.
template<class InputIt, class Value>
bool contains(InputIt begin, InputIt end, Value value) {
	return find(begin, end, value) != end;
}

// Get a random element from a non-empty set.
template<class ElementType>
ElementType randomElement(const set<ElementType>& bin) {
	assert(bin.size() > 0);

	auto it = bin.begin();
	advance(it, rand() % bin.size());

	return *it;
}
