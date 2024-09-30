#ifndef TASK_HPP
#define TASK_HPP

#include "pipelineData.hpp"
#include <memory>

enum class TaskType {
    GraphUpdate,
    MSTComputation,
    Response,
    CREATE,
    ADD,
    SOLVE,
    CommandProcessing
    
};

class task {
public:
    task(TaskType type, std::shared_ptr<pipelineData> data);
    void execute();
    static void enqueueTask(TaskType type, std::shared_ptr<pipelineData> data);

private:
    TaskType type_;
    std::shared_ptr<pipelineData> data_;
};

#endif // TASK_HPP