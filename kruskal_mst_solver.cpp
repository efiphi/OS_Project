// kruskal_mst_solver.cpp

#include "kruskal_mst_solver.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

class DSU {
    std::vector<int> parent, rank;

public:
    DSU(int n) : parent(n, -1), rank(n, 1) {}

    int find(int i) {
        if (parent[i] == -1) return i;
        return parent[i] = find(parent[i]);
    }

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
    std::sort(edges.begin(), edges.end());
    std::vector<Edge> mstEdges;
    DSU dsu(graph.getV());

    for (const Edge& edge : edges) {
        if (dsu.find(edge.v) != dsu.find(edge.w)) {
            dsu.unite(edge.v, edge.w);
            mstEdges.push_back(edge);
        }
    }

    return mstEdges; // Return MST edges
}


std::string KruskalMSTSolver::getMSTResults(Graph& graph, const std::vector<Edge>& mstEdges) {
    std::ostringstream oss;

    oss << "Edges in the constructed MST:\n";
    for (const auto& edge : mstEdges) {
        oss << edge.v << " -- " << edge.w << " == " << edge.weight << "\n";
    }

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
