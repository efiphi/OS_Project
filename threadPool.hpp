#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <mutex>
#include <memory>
#include <thread>               
#include <condition_variable>   


class task;

class threadPool {
public:
    threadPool(size_t numThreads);
    ~threadPool();
    void enqueue(std::shared_ptr<task> task);
    void start();
    void stop();

private:
    std::vector<std::thread> workers;
    std::queue<std::shared_ptr<task>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stopFlag;
    void workerFunction();
};

#endif // THREAD_POOL_HPP