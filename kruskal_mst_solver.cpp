// kruskal_mst_solver.cpp

#include "kruskal_mst_solver.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

class DSU {
    std::vector<int> parent, rank;

public:
    DSU(int n) : parent(n, -1), rank(n, 1) {}

    // Find with path compression
    int find(int i) {
        if (parent[i] == -1) return i;
        return parent[i] = find(parent[i]);
    }

    // Union by rank
    void unite(int x, int y) {
        int s1 = find(x);
        int s2 = find(y);
        if (s1 != s2) {
            if (rank[s1] < rank[s2]) parent[s1] = s2;
            else if (rank[s1] > rank[s2]) parent[s2] = s1;
            else { parent[s2] = s1; rank[s1]++; }
        }
    }
};

std::vector<Edge> KruskalMSTSolver::solveMST(Graph& graph) {
    auto edges = graph.getEdges();
    int V = graph.getV();  // Number of vertices

    // Handle the case of an empty graph
    if (V == 0 || edges.empty()) {
        std::cout << "Graph is empty!" << std::endl;
        return {};
    }

    std::cout << "Number of vertices: " << V << std::endl;
    std::cout << "Edges in the graph:\n";
    for (const auto& edge : edges) {
        std::cout << edge.v << " -- " << edge.w << " == " << edge.weight << std::endl;
    }

    // Sort edges by weight
    std::sort(edges.begin(), edges.end());

    // Disjoint Set Union (DSU) for cycle detection
    DSU dsu(V);
    std::vector<Edge> mstEdges;

    // Iterate over the sorted edges
    for (const Edge& edge : edges) {
        std::cout << "Processing edge: " << edge.v << " -- " << edge.w << " == " << edge.weight << std::endl;
        // Check if the current edge forms a cycle
        if (dsu.find(edge.v) != dsu.find(edge.w)) {
            dsu.unite(edge.v, edge.w);
            mstEdges.push_back(edge);
            std::cout << "Added to MST: " << edge.v << " -- " << edge.w << std::endl;
        }
        // Stop if MST is complete (contains V-1 edges)
        if (mstEdges.size() == V - 1) break;
    }

    // Check if we found a valid MST (if the graph was disconnected, MST will be incomplete)
    if (mstEdges.size() != V - 1) {
        std::cout << "Graph is disconnected! No valid MST found." << std::endl;
        return {};  // Return an empty MST to signify failure
    }

    return mstEdges;
}


std::string KruskalMSTSolver::getMSTResults(Graph& graph, const std::vector<Edge>& mstEdges) {
    std::ostringstream oss;

    // Handle the case where the MST is empty or invalid
    if (mstEdges.empty()) {
        oss << "No valid MST could be constructed from the given graph.\n";
        return oss.str();
    }

    // List edges in the MST
    oss << "Edges in the constructed MST:\n";
    for (const auto& edge : mstEdges) {
        oss << edge.v << " -- " << edge.w << " == " << edge.weight << "\n";
    }

    // Calculate additional statistics about the MST
    int totalWeight = graph.calculateTotalWeight(mstEdges);
    int longestDistance = graph.findLongestDistance(mstEdges);
    int shortestDistance = graph.findShortestDistance(mstEdges);
    double averageDistance = graph.calculateAverageDistance(mstEdges);

    oss << "Total weight of the MST: " << totalWeight << std::endl;
    oss << "Longest distance between two vertices: " << longestDistance << std::endl;
    oss << "Shortest distance between two vertices: " << shortestDistance << std::endl;
    oss << "Average distance between two vertices: " << averageDistance << std::endl;

    return oss.str();
}