#include "graph.hpp"

Graph::Graph(vector<vector<int>> adjMat): adjMat(adjMat) {
    numVertices = adjMat.size();
    numEdges = 0;
    for (int i = 0; i < numVertices; i++) {
        for(int j=0;j<numVertices;j++){
            if(adjMat.at(i).at(i)!=0){
                throw invalid_argument("Diagonal elements must be zero");
            }
            if(adjMat.at(i).at(j)!=0){
            numEdges++;
        }
    }
}
}
int Graph::getNumEdges(){
    return numEdges;
}
int Graph::getNumVertices(){
    return numVertices;
}
vector<vector<int>> Graph::getAdjMat(){
    return adjMat;
}
void Graph::addEdge(int from, int to, int weight){
    if (from < 0 || from >= numVertices || to < 0 || to >= numVertices||from==to) {
        throw invalid_argument("Invalid vertex index");
    }
    adjMat.at(from).at(to) = weight;
}
void Graph::removeEdge(int from, int to){
    if (from < 0 || from >= numVertices || to < 0 || to >= numVertices||from==to) {
        throw invalid_argument("Invalid vertex index");
    }
    adjMat.at(from).at(to) = 0;
}   