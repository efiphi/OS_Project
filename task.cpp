#include "task.hpp"
#include "graph.hpp"
#include "graphParsingStage.hpp"
#include "graphUpdateStage.hpp"
#include "mstComputationStage.hpp"
#include "responseStage.hpp"
#include "server.hpp"
#include <sstream>
#include <iostream>
#include <unistd.h>

task::task(TaskType type, std::shared_ptr<pipelineData> data)
    : type_(type), data_(data) {}

void task::execute() {
    std::cout << "Executing task of type: " << static_cast<int>(type_) << " with FD: " << data_->client_fd << std::endl;

    switch (type_) {
        case TaskType::GraphUpdate:
            {
                graphUpdateStage updateStage;
                updateStage.process(data_);
            }
            break;

        case TaskType::MSTComputation:
            {
                mstComputationStage mstStage;
                mstStage.process(data_);
            }
            break;

        case TaskType::Response:
            {
                responseStage responseStage;
                responseStage.process(data_);
            }
            break;

        default:
            std::cerr << "Unknown task type: " << static_cast<int>(type_) << std::endl;
            break;
    }
}

void task::enqueueTask(TaskType type, std::shared_ptr<pipelineData> data) {
    std::cout << "Enqueuing task of type: " << static_cast<int>(type) << " for FD: " << data->client_fd << std::endl;
    
    switch (type) {
        case TaskType::CommandProcessing:
            server::getInstance().commandProcessing.enqueueTask([data]() {
                // Define the task's logic here for processing commands
            });
            break;

        case TaskType::GraphUpdate:
            server::getInstance().graphUpdate.enqueueTask([data]() {
                // Define the task's logic here for updating the graph
            });
            break;

        case TaskType::MSTComputation:
            server::getInstance().mstComputation.enqueueTask([data]() {
                // Define the task's logic here for computing MST
            });
            break;

        case TaskType::Response:
            server::getInstance().response.enqueueTask([data]() {
                // Define the task's logic here for sending the response to the client
            });
            break;

        default:
            std::cerr << "Unknown task type!" << std::endl;
            break;
    }
}
