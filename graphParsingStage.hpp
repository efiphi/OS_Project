#ifndef GRAPH_PARSING_STAGE_HPP
#define GRAPH_PARSING_STAGE_HPP

#include "pipelineStage.hpp"
#include "pipelineData.hpp"

class graphParsingStage : public pipelineStage {
public:
    void process(std::shared_ptr<pipelineData> data) override {
        // Parse the command and initialize the graph (e.g., "create <V> <E>")
        if (data->command == "create") {
            // Parse vertices and edges from data
            data->result = "Graph created with " + std::to_string(data->vertices) + " vertices.";
        }

        // Pass to the next stage
        if (nextStage) {
            nextStage->process(data);
        }
    }
};

#endif // GRAPH_PARSING_STAGE_HPP
