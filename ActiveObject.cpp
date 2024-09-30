#include "ActiveObject.hpp"

ActiveObject::ActiveObject() : stopFlag(false) {
    // Start the worker thread that will process tasks
    workerThread = std::thread(&ActiveObject::processTasks, this);
}

ActiveObject::~ActiveObject() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopFlag = true;
    }
    condition.notify_all(); // Wake up the worker thread to exit
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void ActiveObject::enqueueTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(std::move(task)); // Push the task into the queue
    }
    condition.notify_one(); // Notify the worker thread that a new task is available
}

void ActiveObject::processTasks() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return !taskQueue.empty() || stopFlag; });

            if (stopFlag && taskQueue.empty()) {
                return; // Exit the loop if stopFlag is set and there are no tasks left
            }

            task = std::move(taskQueue.front());
            taskQueue.pop();
        }
        task(); // Execute the task outside the locked region
    }
}