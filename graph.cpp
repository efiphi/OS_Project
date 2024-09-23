#include "graph.hpp"
#include <algorithm>
#include <numeric>
#include <climits>
#include <queue>
#include <vector>
#include <limits.h>

Graph::Graph(int V) : V(V), adj(V) {}

void Graph::addEdge(int v, int w, int weight) {
    if (v >= V || w >= V) {
        throw std::out_of_range("Vertex out of range");
    }
    adj[v].push_back(Edge(v, w, weight));
    adj[w].push_back(Edge(w, v, weight));
    edges.push_back(Edge(v, w, weight));
}

void Graph::removeEdge(int v, int w) {
    if (v >= V || w >= V) {
        throw std::out_of_range("Vertex out of range");
    }

    // Remove both v->w and w->v edges for undirected graph
    edges.erase(std::remove_if(edges.begin(), edges.end(), [v, w](Edge const& edge) {
        return (edge.v == v && edge.w == w) || (edge.v == w && edge.w == v);
    }), edges.end());

    std::cout << "Edge removed between " << v << " and " << w << std::endl;
}

int Graph::getV() const {
    return V;
}

const std::vector<Edge>& Graph::getEdges() const {
    return edges;
}

const std::vector<std::vector<Edge>>& Graph::getAdj() const {
    return adj;
}

void Graph::printGraph() const {
    for (int v = 0; v < V; ++v) {
        std::cout << v << ": ";
        for (const Edge& edge : adj[v]) {
            std::cout << edge.w << " (" << edge.weight << ") ";
        }
        std::cout << std::endl;
    }
}

// Helper function for Prim's algorithm to find the vertex with the minimum key
int Graph::minKey(const std::vector<int>& key, const std::vector<bool>& inMST) const {
    int min = INT_MAX, min_index = -1;

    for (int v = 0; v < V; ++v) {
        if (!inMST[v] && key[v] < min) {
            min = key[v];
            min_index = v;
        }
    }
    return min_index;
}

// Calculate total weight of MST
int Graph::calculateTotalWeight(const std::vector<Edge>& mstEdges) const {
    return std::accumulate(mstEdges.begin(), mstEdges.end(), 0, 
        [](int sum, const Edge& edge) {
            return sum + edge.weight;
        }
    );
}

// Find the longest edge in the MST
int Graph::findLongestDistance(const std::vector<Edge>& mstEdges) const {
    // Create adjacency list for the MST
    std::vector<std::vector<Edge>> mstAdj(V);

    
    for (const Edge& edge : mstEdges) {
        mstAdj[edge.v].push_back(edge);
        mstAdj[edge.w].push_back(Edge(edge.w, edge.v, edge.weight)); // Add reverse direction
        
    }

    // Step 1: Run BFS from any vertex (let's start from vertex 0)
    auto [farthestVertex, _firstDistance] = bfs(0, mstAdj);

    // Step 2: Run BFS from the farthest vertex found to get the longest path
    auto [_secondVertex, maxDist] = bfs(farthestVertex, mstAdj);

    return maxDist; // This is the longest distance in the MST (diameter)
}



double Graph::calculateAverageDistance(const std::vector<Edge>& mstEdges) const {
    // Create adjacency list for the MST
    std::vector<std::vector<Edge>> mstAdj(V);

    for (const Edge& edge : mstEdges) {
        mstAdj[edge.v].push_back(edge); // Forward edge
        mstAdj[edge.w].push_back(Edge(edge.w, edge.v, edge.weight)); // Reverse direction
    }

    // Initialize distance matrix for the MST
    std::vector<std::vector<int>> dist(V, std::vector<int>(V, INT_MAX));
    for (int i = 0; i < V; i++) {
        dist[i][i] = 0; // Distance to self is 0
    }

    // Fill distances based on MST edges
    for (const Edge& edge : mstEdges) {
        dist[edge.v][edge.w] = edge.weight;
        dist[edge.w][edge.v] = edge.weight; // Undirected
    }

    // Floyd-Warshall algorithm to compute shortest paths between all pairs
    for (int k = 0; k < V; ++k) {
        for (int i = 0; i < V; ++i) {
            for (int j = 0; j < V; ++j) {
                if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX) {
                    dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
                }
            }
        }
    }

    // Sum all distances for unique pairs (i < j)
    int totalDistance = 0;
    int totalPairs = 0;

    for (int i = 0; i < V; ++i) {
        for (int j = i + 1; j < V; ++j) {
            if (dist[i][j] != INT_MAX) { // Ignore pairs not connected
                totalDistance += dist[i][j];
                totalPairs++;
            }
        }
    }

    // Calculate and return the average distance
    return totalPairs > 0 ? static_cast<double>(totalDistance) / totalPairs : 0.0;
}




// Find the shortest edge in the MST
int Graph::findShortestDistance(const std::vector<Edge>& mstEdges) const {
    auto minEdge = std::min_element(mstEdges.begin(), mstEdges.end(),
        [](const Edge& a, const Edge& b) {
            return a.weight < b.weight;
        }
    );
    return minEdge != mstEdges.end() ? minEdge->weight : 0;
}

std::pair<int, int> Graph::bfs(int startVertex, const std::vector<std::vector<Edge>>& mstAdj) const {
    std::vector<int> dist(V, INT_MAX); // Distance from the start vertex
    dist[startVertex] = 0;

    std::queue<int> q;
    q.push(startVertex);

    int farthestVertex = startVertex;
    int maxDist = 0;

    while (!q.empty()) {
        int u = q.front();
        q.pop();


        for (const Edge& edge : mstAdj[u]) {
            int v = edge.w;
            if (dist[v] == INT_MAX) { // If not visited
                dist[v] = dist[u] + edge.weight; // Update distance
                q.push(v);


                if (dist[v] > maxDist) {
                    maxDist = dist[v];
                    farthestVertex = v; // Keep track of farthest vertex
                }
            }
        }
    }


    return {farthestVertex, maxDist};
}