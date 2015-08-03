#include "PushLiftImpl1.h"
#include <algorithm>
#include <cassert>
#include <exception>

int PushLiftImpl1::getHeight(const int& nodeNo) const
{
  return H[nodeNo];
}

void PushLiftImpl1::setHeight(const int& nodeNo, const int& newHeight)
{
  H[nodeNo] = newHeight;
}

weight_t PushLiftImpl1::getExcess(const int& nodeNo) const
{
  return D[nodeNo];
}

void PushLiftImpl1::setExcess(const int& nodeNo, const weight_t& newWeight)
{
  D[nodeNo] = newWeight;
}

void PushLiftImpl1::addEdge(const pair<int, int>& conn, weight_t weight)
{
  assert(weight >= 0);

  auto& edgeList = edges[conn.first];
  for (pair<int, weight_t>& existingEdge : edgeList) {
  	if (existingEdge.first == conn.second) {
  		existingEdge.second += weight;
  		return;
  	}
  }

  edgeList.push_back(make_pair(conn.second, weight));
}

void PushLiftImpl1::initDataStructure(const int& maxNode, const int&)
{
  const unsigned int requiredSize = maxNode + 1; // Slightly too large, for one-indexing.
  edges = vector<vector<EdgeWeight>>(requiredSize);
  H = vector<int>(requiredSize, 0);
  D = vector<weight_t>(requiredSize, 0);
}

weight_t PushLiftImpl1::getCapacity(const int& from, const int& to) const
{
  for (const auto& edge : edges[from]) {
    if (edge.first == to) {
      return edge.second;
    }
  }

  throw out_of_range("No edge found like that.");
}

void PushLiftImpl1::setCapacity(const int& from, const int& to, const weight_t& newCapacity)
{
  for (auto& edge : edges[from]) {
    if (edge.first == to) {
      edge.second = newCapacity;
      return;
    }
  }

  throw out_of_range("No edge found like that.");
}

vector<int> PushLiftImpl1::getNeighbours(int nodeNo) const
{
  vector<int> result(edges[nodeNo].size());
  for (unsigned int i = 0; i < edges[nodeNo].size(); i++) {
    result[i] = edges[nodeNo][i].first;
  }

  return result;
}
