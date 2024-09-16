#include "mst_solver.hpp"
#include <sstream>

std::string MSTSolver::getMSTResults(Graph& graph, const std::vector<Edge>& mstEdges) {
    int totalWeight = graph.calculateTotalWeight(mstEdges);
    int longestDistance = graph.findLongestDistance(mstEdges);
    int shortestDistance = graph.findShortestDistance(mstEdges);
    double averageDistance = graph.calculateAverageDistance(mstEdges);

    std::stringstream ss;
    ss << "Total weight of the MST: " << totalWeight << "\n";
    ss << "Longest distance between two vertices: " << longestDistance << "\n";
    ss << "Shortest distance between two vertices: " << shortestDistance << "\n";
    ss << "Average distance between two vertices: " << averageDistance << "\n";

    return ss.str();
}
