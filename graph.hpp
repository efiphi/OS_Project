#pragma once
#include <iostream>
#include <vector>
using namespace std;
// this is a calss for a weighted directed Graph
class Graph {
    vector<vector<int>> adjMat;
    int numVertices;
    int numEdges;
public:
Graph(vector<vector<int>> adjMat);
vector<vector<int>> getAdjMat();
int getNumVertices();
int getNumEdges();
void addEdge(int from, int to, int weight);
void removeEdge(int from, int to);
};