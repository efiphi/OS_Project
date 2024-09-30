#ifndef ACTIVE_OBJECT_HPP
#define ACTIVE_OBJECT_HPP

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ActiveObject {
public:
    ActiveObject();
    ~ActiveObject();

    // Enqueue a new task (a function) to be processed by the ActiveObject's thread
    void enqueueTask(std::function<void()> task);

private:
    void processTasks(); // Method for the worker thread to process the tasks

    std::queue<std::function<void()>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::thread workerThread;
    std::atomic<bool> stopFlag;
};

#endif // ACTIVE_OBJECT_HPP