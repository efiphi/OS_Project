#ifndef TASK_QUEUE_HPP
#define TASK_QUEUE_HPP

#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

// A thread-safe task queue for managing tasks
class TaskQueue {
public:
    // Add a new task to the queue
    void enqueueTask(const std::function<void()>& task) {
        std::unique_lock<std::mutex> lock(mutex_);
        taskQueue_.push(task);
        condition_.notify_one();  // Notify one waiting thread
    }

    // Fetch a task from the queue
    std::function<void()> dequeueTask() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this]() { return !taskQueue_.empty(); });
        std::function<void()> task = taskQueue_.front();
        taskQueue_.pop();
        return task;
    }

    // Check if the queue is empty
    bool isEmpty() {
        std::unique_lock<std::mutex> lock(mutex_);
        return taskQueue_.empty();
    }

private:
    std::queue<std::function<void()>> taskQueue_; // Queue holding tasks
    std::mutex mutex_;                            // Mutex for thread safety
    std::condition_variable condition_;           // Condition variable for task synchronization
};

#endif // TASK_QUEUE_HPP