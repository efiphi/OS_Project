#include "prim_mst_solver.hpp"
#include <vector>
#include <limits.h>
#include <iostream>
#include <sstream> 

std::vector<Edge> PrimMSTSolver::solveMST(Graph& graph) {
    int V = graph.getV();
    std::vector<int> key(V, INT_MAX);
    std::vector<int> parent(V, -1);
    std::vector<bool> inMST(V, false);
    std::vector<Edge> mstEdges;

    key[0] = 0;

    for (int count = 0; count < V - 1; ++count) {
        int u = graph.minKey(key, inMST);
        inMST[u] = true;

        for (const Edge& edge : graph.getAdj()[u]) {
            int v = edge.w;
            if (!inMST[v] && edge.weight < key[v]) {
                key[v] = edge.weight;
                parent[v] = u;
            }
        }
    }

    for (int i = 1; i < V; ++i) {
        for (const Edge& edge : graph.getAdj()[i]) {
            if (edge.w == parent[i]) {
                mstEdges.push_back(edge);
                break;
            }
        }
    }

    return mstEdges; // Return the MST edges
}


std::string PrimMSTSolver::getMSTResults(Graph& graph, const std::vector<Edge>& mstEdges) {
    std::ostringstream oss;

     oss << "Edges in the constructed MST:\n";
    for (const auto& edge : mstEdges) {
        oss << edge.v << " - " << edge.w << " == " << edge.weight << "\n";
    }

    // Calculate the desired values
    int totalWeight = graph.calculateTotalWeight(mstEdges);
    int longestDistance = graph.findLongestDistance(mstEdges);
    int shortestDistance = graph.findShortestDistance(mstEdges);
    double averageDistance = graph.calculateAverageDistance(mstEdges);

    // Output the calculated values
    oss << "Total weight of the MST: " << totalWeight << "\n";
    oss << "Longest distance between two vertices: " << longestDistance << "\n";
    oss << "Shortest distance between two vertices: " << shortestDistance << "\n";
    oss << "Average distance between two vertices: " << averageDistance << "\n";

    return oss.str();
}
