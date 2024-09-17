#ifndef GRAPH_PARSING_STAGE_HPP
#define GRAPH_PARSING_STAGE_HPP

#include "pipelineStage.hpp"
#include "pipelineData.hpp"

class graphParsingStage : public pipelineStage {
public:
    void process(std::shared_ptr<pipelineData> data) override {
        // Only handle "create" command
        if (data->command == "create") {
            // Initialize the graph
            data->graph = Graph(data->vertices);
            data->response = "Graph created with " + std::to_string(data->vertices) + " vertices and " + std::to_string(data->edges) + " edges.";
            std::cout << "Debug: Graph created with " << data->vertices << " vertices and " << data->edges << " edges." << std::endl;

            // Pass to the next stage
            if (nextStage) {
                nextStage->process(data);
            }
        }
    }
};

#endif // GRAPH_PARSING_STAGE_HPP
