#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <iostream>
#include <algorithm>

class Edge {
public:
    int v, w; // Vertices
    int weight; // Edge weight

    Edge(int v, int w, int weight) : v(v), w(w), weight(weight) {}

    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

class Graph {
public:
    Graph(int V);

    void addEdge(int v, int w, int weight);
    void removeEdge(int v, int w);

    int getV() const;
    const std::vector<Edge>& getEdges() const;
    const std::vector<std::vector<Edge>>& getAdj() const;

    // Method to print the graph
    void printGraph() const;

    int minKey(const std::vector<int>& key, const std::vector<bool>& inMST) const;

    // Calculate total, longest, and shortest distances in MST
    int calculateTotalWeight(const std::vector<Edge>& mstEdges) const;
    int findLongestDistance(const std::vector<Edge>& mstEdges) const;
    int findShortestDistance(const std::vector<Edge>& mstEdges) const;
    double calculateAverageDistance(const std::vector<Edge>& mstEdges) const;
    
    // Helper method for BFS to find the farthest vertex and distance
    std::pair<int, int> bfs(int startVertex, const std::vector<std::vector<Edge>>& mstAdj) const;


private:
    int V; // Number of vertices
    std::vector<std::vector<Edge>> adj; // Adjacency list
    std::vector<Edge> edges; // Edge list
};

#endif // GRAPH_HPP