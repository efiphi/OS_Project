#include "graphUpdateStage.hpp"

void graphUpdateStage::process(std::shared_ptr<pipelineData> data) {
    // Handle graph updates (e.g., "add <v1> <v2>")
    if (data->command == "add") {
        // Add edge to graph data
        // This is a placeholder for actual implementation
        data->result = "Edge added.";
    }

    // Pass to the next stage
    if (nextStage) {
        nextStage->process(data);
    }
}
