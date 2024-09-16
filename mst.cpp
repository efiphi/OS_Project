#include "mst.hpp"

MST::MST(int V) : V(V), totalWeight(0) {}

void MST::addEdge(const Edge& edge) {
    mstEdges.push_back(edge);
    totalWeight += edge.weight; // Update the total weight of the MST
}

int MST::getTotalWeight() const {
    return totalWeight;
}

const std::vector<Edge>& MST::getTreeEdges() const {
    return mstEdges;
}

void MST::printMST() const {
    std::cout << "Minimum Spanning Tree:" << std::endl;
    for (const Edge& edge : mstEdges) {
        std::cout << edge.v << " --(" << edge.weight << ")--> " << edge.w << std::endl;
    }
    std::cout << "Total weight of the MST: " << totalWeight << std::endl;
}
