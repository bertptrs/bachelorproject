#include "PushLiftImpl3.hpp"

#include <algorithm>
#include <cassert>
#include <exception>

int PushLiftImpl3::getHeight(const int& nodeNo) const
{
  return H[nodeNo];
}

void PushLiftImpl3::setHeight(const int& nodeNo, const int& newHeight)
{
  H[nodeNo] = newHeight;
}

weight_t PushLiftImpl3::getExcess(const int& nodeNo) const
{
  return D[nodeNo];
}

void PushLiftImpl3::setExcess(const int& nodeNo, const weight_t& newWeight)
{
  D[nodeNo] = newWeight;
}

void PushLiftImpl3::addEdge(const pair<int, int>& conn, weight_t weight)
{
  assert(weight >= 0);

  edges.push_back({conn, weight});
}

void PushLiftImpl3::initDataStructure(const int& maxNode, const int& numEdges)
{
  const unsigned int requiredSize = maxNode + 3; // Slightly too large, for one-indexing.
  edges = vector<Edge>();
  edges.reserve(numEdges);
  H = vector<int>(requiredSize, 0);
  D = vector<weight_t>(requiredSize, 0);
}

weight_t PushLiftImpl3::getCapacity(const int& from, const int& to) const
{
  for (auto& edge : edges) {
    if (edge.first.first == from && edge.first.second == to) {
      return edge.second;
    }
  }

  throw out_of_range("No edge found like that.");
}

void PushLiftImpl3::setCapacity(const int& from, const int& to, const weight_t& newCapacity)
{
  for (auto& edge : edges) {
    if (edge.first.first == from && edge.first.second == to) {
      edge.second = newCapacity;
      return;
    }
  }

  throw out_of_range("No edge found like that.");
}

vector<int> PushLiftImpl3::getNeighbours(int nodeNo) const
{
  vector<int> result;
  for (auto& edge : edges) {
    if (edge.first.first == nodeNo) {
      result.push_back(edge.first.second); // Yay for O(N)
    }
  }

  return result;
}
