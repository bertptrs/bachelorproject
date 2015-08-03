#ifndef PUSHLIFT_IMPL_1_H
#define PUSHLIFT_IMPL_1_H

#include "PushLift.h"

class PushLiftImpl1 : public PushLift
{
  private:
    typedef pair<int, weight_t> EdgeWeight;

    vector<int> H; // Height of node
		vector<weight_t> D; // Excess of node
		vector<vector<EdgeWeight>> edges; // Edges with remaining capacity.
										// Stored as an adjecency list with weights.

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

#endif
