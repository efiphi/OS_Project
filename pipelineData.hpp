#ifndef PIPELINEDATA_HPP
#define PIPELINEDATA_HPP

#include <memory>
#include <string>
#include "graph.hpp"

class pipelineData {
public:
    // Graph-related members
    Graph graph;
    int edges;
    int vertices;
    int v, w, weight;
    int client_fd;

    pipelineData() : graph(0), v(0), w(0), weight(0), client_fd(-1) {}

    // MST computation
    std::string algorithm;

    // Response to be sent back to the client
    std::string response;

    // Command type (create, add, etc.)
    std::string command;

};

#endif // PIPELINEDATA_HPP
