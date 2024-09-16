#include "mstComputationStage.hpp"

void mstComputationStage::process(std::shared_ptr<pipelineData> data) {
    // Solve MST using the specified algorithm
    if (data->command == "solve") {
        // Use MSTFactory to solve the graph
        // This is a placeholder for actual implementation
        data->result = "MST solved. Total weight: ...";
    }

    // Pass to the next stage
    if (nextStage) {
        nextStage->process(data);
    }
}
