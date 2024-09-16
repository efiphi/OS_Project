#ifndef TASK_HPP
#define TASK_HPP

#include <memory>
#include <functional>
#include "pipelineStage.hpp"
#include "pipelineData.hpp"

class task {
public:
    // Constructor for pipeline stages
    task(std::shared_ptr<pipelineStage> pipeline, std::shared_ptr<pipelineData> data);

    // Constructor for lambda functions
    task(std::function<void()> func);

    // Method to execute the task
    void execute();

private:
    // Member variables
    std::shared_ptr<pipelineStage> pipeline_;
    std::shared_ptr<pipelineData> data_;
    std::function<void()> func_;
};

#endif // TASK_HPP
