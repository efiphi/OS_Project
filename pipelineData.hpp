#ifndef PIPELINE_DATA_HPP
#define PIPELINE_DATA_HPP

#include <string>
#include <vector>

class pipelineData {
public:
    // Placeholder data members
    std::string command;               // Command type (e.g., "create", "add", "remove", "solve")
    int vertices;                      // Number of vertices in the graph
    std::vector<std::tuple<int, int, int>> edges;  // Graph edges (from, to, weight)
    std::string result;                // Result to be sent back to the client

    
};

#endif // PIPELINE_DATA_HPP
