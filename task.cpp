#include "task.hpp"
#include <iostream>

// Constructor for pipeline stages
task::task(std::shared_ptr<pipelineStage> pipeline, std::shared_ptr<pipelineData> data)
    : pipeline_(pipeline), data_(data), func_(nullptr) {}

// New constructor for lambda functions
task::task(std::function<void()> func)
    : pipeline_(nullptr), data_(nullptr), func_(func) {}

// Execute method
void task::execute() {
    if (func_) {
        // If the task was created with a lambda function, execute it
        func_();
    } else if (pipeline_ && data_) {
        // Original pipeline processing
        pipeline_->process(data_);
    } else {
        std::cerr << "Task is not properly initialized." << std::endl;
    }
}
