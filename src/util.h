#include <algorithm>

using namespace std;

template<class InputIt, class Value>
bool contains(InputIt begin, InputIt end, Value value) {
	return find(begin, end, value) != end;
}
