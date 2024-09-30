#include "threadPool.hpp"
#include "task.hpp"

threadPool::threadPool(size_t numThreads) : stopFlag(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&threadPool::workerFunction, this);
    }
}

threadPool::~threadPool() {
    stop();
}

void threadPool::enqueue(std::shared_ptr<task> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(task);
    }
    condition.notify_one();
}

void threadPool::start() {
    // Threads are started in the constructor
}

void threadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stopFlag = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void threadPool::workerFunction() {
    while (true) {
        std::shared_ptr<task> currentTask;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return !tasks.empty() || stopFlag; });
            if (stopFlag && tasks.empty()) {
                return;
            }
            currentTask = tasks.front();
            tasks.pop();
        }
        if (currentTask) {
            currentTask->execute();
        }
    }
}