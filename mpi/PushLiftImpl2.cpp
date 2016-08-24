#include "PushLiftImpl2.hpp"
#include <cassert>
#include <exception>

int PushLiftImpl2::getHeight(const int& nodeNo) const
{
  try {
    return nodeStates.at(nodeNo).first;
  } catch (out_of_range ex) {
    return 0;
  }
}

void PushLiftImpl2::setHeight(const int& nodeNo, const int& newHeight)
{
  nodeStates[nodeNo].first = newHeight;
}

weight_t PushLiftImpl2::getExcess(const int& nodeNo) const
{
  try {
    return nodeStates.at(nodeNo).second;
  } catch (out_of_range ex) {
    return 0;
  }
}

void PushLiftImpl2::setExcess(const int& nodeNo, const weight_t& newWeight)
{
  nodeStates[nodeNo].second = newWeight;
}

void PushLiftImpl2::initDataStructure(const int&, const int&)
{
  // No initialization required, map will initialize when needed.
}

void PushLiftImpl2::addEdge(const pair<int, int>& conn, weight_t weight)
{
  assert(weight >= 0);

  edges[conn.first][conn.second] += weight;
}

weight_t PushLiftImpl2::getCapacity(const int& from, const int& to) const
{
  return edges.at(from).at(to);
}

void PushLiftImpl2::setCapacity(const int& from, const int& to, const weight_t& newCapacity)
{
  edges[from][to] = newCapacity;
}

vector<int> PushLiftImpl2::getNeighbours(int nodeNo) const
{
  auto edgeList = edges.find(nodeNo);
  if (edgeList == edges.end()) {
    return {};
  }

  vector<int> result;

  for (auto& edge : edges.at(nodeNo)) {
    result.push_back(edge.first);
  }

  return result;
}
