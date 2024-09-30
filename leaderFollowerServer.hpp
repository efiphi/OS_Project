#ifndef SERVER_HPP
#define SERVER_HPP

#include "pipelineData.hpp" // Assuming this header defines pipelineData class
#include "mst_solver.hpp"    // For MST solvers
#include "taskQueue.hpp"    // A task queue to manage tasks in the thread pool
#include <memory>
#include <vector>
#include <string>
#include <thread>

// Enum for representing the MST algorithms available
enum MSTAlgorithmType {
    PRIM,
    KRUSKAL
};

// Forward declarations
class ThreadPool;

// Server class
class server {
public:
    // Constructor
    explicit server(int port);

    // Start the server
    void start();

    // Stop the server
    void stop();

private:
    // Handle client requests
    void handleClient(int client_fd, std::shared_ptr<pipelineData> data);

    // Process MST creation command
    void processCreateCommand(std::istringstream& iss, std::shared_ptr<pipelineData> data);

    // Process edge addition command
    void processAddCommand(std::istringstream& iss, std::shared_ptr<pipelineData> data);

    // Process MST solving command
    void processSolveCommand(std::istringstream& iss, std::shared_ptr<pipelineData> data);

    int port;  // Port to run the server on

    ThreadPool* leaderFollowerThreadPool;  // Thread pool for handling tasks
};

// ThreadPool class for the Leader-Follower pattern
class ThreadPool {
public:
    explicit ThreadPool(int numThreads);
    ~ThreadPool();

    // Method to enqueue a task
    void enqueueTask(const std::function<void()>& task);

    // Method to start the thread pool
    void start();

    // Method to stop the thread pool
    void stop();

private:
    // Worker function for each thread
    void workerThread();

    std::vector<std::thread> workers;     // List of worker threads
    std::deque<std::function<void()>> taskQueue; // Task queue
    std::mutex queueMutex;                // Mutex for task queue
    std::condition_variable condition;    // Condition variable for synchronization
    bool stopAll;                         // Flag to stop all threads
};

#endif // SERVER_HPP