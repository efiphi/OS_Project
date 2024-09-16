#ifndef MST_HPP
#define MST_HPP

#include "graph.hpp"
#include <vector>
#include <iostream>

class MST {
public:
    // Constructor to initialize the MST for a graph with V vertices
    MST(int V);

    // Add an edge to the MST
    void addEdge(const Edge& edge);

    // Get total weight of the MST
    int getTotalWeight() const;

    // Get all edges in the MST
    const std::vector<Edge>& getTreeEdges() const;

    // Print the MST
    void printMST() const;

private:
    int V; // Number of vertices in the graph
    std::vector<Edge> mstEdges; // Edges that are part of the MST
    int totalWeight; // Total weight of the MST
};

#endif // MST_HPP
