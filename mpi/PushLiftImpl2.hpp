#ifndef PUSHLIFT_IMPL_2_HPP
#define PUSHLIFT_IMPL_2_HPP

#include "PushLift.hpp"
#include <map>

using namespace std;

/**
 * This implementation uses the std::map for storage and access.
 * This is both slower and faster than a vector, as it has logarithmic access times.
 *
 * Not sure whether this one can be easily generated though.
 */
class PushLiftImpl2 : public PushLift
{
private:
  map<int, map<int, weight_t>> edges;
  map<int, pair<int, weight_t>> nodeStates; // Heights and excesses.

protected:
  virtual void addEdge(const pair<int, int>& conn, weight_t weight);
  virtual weight_t getExcess(const int& nodeNo) const;
  virtual void setExcess(const int& nodeNo, const weight_t& newWeight);
  virtual int getHeight(const int& nodeNo) const;
  virtual void setHeight(const int& nodeNo, const int& newHeight);
  virtual vector<int> getNeighbours(int node) const;
  virtual weight_t getCapacity(const int& from, const int& to) const;
  virtual void setCapacity(const int& from, const int& to, const weight_t& newCapacity);
  virtual void initDataStructure(const int& maxNode, const int& numEdges);

public:
  using PushLift::PushLift;
};

#endif /* end of include guard: PUSHLIFT_IMPL_2_HPP */
