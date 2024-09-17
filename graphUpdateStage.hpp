#ifndef GRAPH_UPDATE_STAGE_HPP
#define GRAPH_UPDATE_STAGE_HPP

#include "pipelineStage.hpp"
#include "pipelineData.hpp"
#include "task.hpp"

class graphUpdateStage : public pipelineStage {
public:
    void process(std::shared_ptr<pipelineData> data) override {
        if (data->command == "add") {
            // Add an edge to the graph
            data->graph.addEdge(data->v, data->w, data->weight);
            data->response = "Edge added from " + std::to_string(data->v) + " to " + std::to_string(data->w) + " with weight " + std::to_string(data->weight) + ".\n";
            std::cout << "Debug: Edge added from " << data->v << " to " << data->w << " with weight " << data->weight << "." << std::endl;

            // Enqueue response task
            task::enqueueTask(TaskType::Response, data);
        }
    }

    ~graphUpdateStage() override = default;
};

#endif // GRAPH_UPDATE_STAGE_HPP
