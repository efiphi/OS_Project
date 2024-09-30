#include "mst_solver.hpp"
#include "mst_factory.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <functional>

// Represents data shared between threads for a client
struct pipelineData {
    int client_fd;
    std::string command;
    std::string response;
    Graph graph;
    std::string algorithm;
    pipelineData() : graph(0) {}

};

// Leader-Follower thread pool implementation
class LeaderFollowerThreadPool {
private:
    std::queue<std::function<void()>> taskQueue;
    std::vector<std::thread> threads;
    std::mutex queueMutex;
    std::condition_variable taskAvailable;
    bool shutdown = false;

public:
    LeaderFollowerThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back(&LeaderFollowerThreadPool::workerThread, this);
        }
    }

    ~LeaderFollowerThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            shutdown = true;
        }
        taskAvailable.notify_all();

        for (auto &t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void addTask(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            taskQueue.push(task);
        }
        taskAvailable.notify_one();  // Notify one waiting thread
    }

private:
    void workerThread() {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(queueMutex);
                taskAvailable.wait(lock, [this]() { return !taskQueue.empty() || shutdown; });

                if (shutdown && taskQueue.empty()) {
                    return;
                }

                task = taskQueue.front();
                taskQueue.pop();
            }

            if (task) {
                task();
            }
        }
    }
};

// Server class definition
class server {
public:
    server(int port);
    void start();
    void stop();

private:
    int port;
    LeaderFollowerThreadPool threadPool {4};  // Pool with 4 threads

    void handleClient(int client_fd, std::shared_ptr<pipelineData> data);
};

server::server(int port) : port(port) {}

void server::start() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        return;
    }

    std::cout << "Server is listening on port " << port << std::endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        std::cout << "Accepted client connection. Client FD: " << client_fd << std::endl;

        auto data = std::make_shared<pipelineData>();
        data->client_fd = client_fd;

        threadPool.addTask([this, client_fd, data]() { handleClient(client_fd, data); });
    }

    close(server_fd);
}

void server::stop() {
    std::cout << "Stopping server..." << std::endl;
}

void server::handleClient(int client_fd, std::shared_ptr<pipelineData> data) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            std::string command(buffer);
            std::cout << "Received command: " << command << std::endl;

            std::istringstream iss(command);
            std::string cmd;
            iss >> cmd;
            data->command = cmd;

            if (cmd == "create") {
                int V, E;
                if (iss >> V >> E) {
                    data->graph = Graph(V);
                    data->response = "Graph created with " + std::to_string(V) + " vertices and " + std::to_string(E) + " edges.\n";
                    threadPool.addTask([data]() {
                        write(data->client_fd, data->response.c_str(), data->response.length());
                    });
                } else {
                    data->response = "Invalid input for create command.\n";
                    threadPool.addTask([data]() {
                        write(data->client_fd, data->response.c_str(), data->response.length());
                    });
                }
            } else if (cmd == "add") {
                int v, w, weight;
                if (iss >> v >> w >> weight) {
                    data->graph.addEdge(v, w, weight);
                    data->response = "Edge added: " + std::to_string(v) + " -> " + std::to_string(w) + " with weight " + std::to_string(weight) + ".\n";
                    threadPool.addTask([data]() {
                        write(data->client_fd, data->response.c_str(), data->response.length());
                    });
                } else {
                    data->response = "Invalid input for add command.\n";
                    threadPool.addTask([data]() {
                        write(data->client_fd, data->response.c_str(), data->response.length());
                    });
                }
            } else if (cmd == "solve") {
                std::string algo;
                if (iss >> algo) {
                    data->algorithm = algo;

                    threadPool.addTask([this, data]() {
                        MSTAlgorithmType algoType = (data->algorithm == "prim") ? PRIM : KRUSKAL;
                        auto solver = MSTFactory::createSolver(algoType);

                        std::vector<Edge> mstEdges = solver->solveMST(data->graph);

                        data->response = solver->getMSTResults(data->graph, mstEdges);
                        threadPool.addTask([data]() {
                            write(data->client_fd, data->response.c_str(), data->response.length());
                        });
                    });
                } else {
                    data->response = "Invalid input for solve command.\n";
                    threadPool.addTask([data]() {
                        write(data->client_fd, data->response.c_str(), data->response.length());
                    });
                }
            } else {
                data->response = "Unknown command.\n";
                threadPool.addTask([data]() {
                    write(data->client_fd, data->response.c_str(), data->response.length());
                });
            }
        } else {
            if (bytes_read == 0) {
                std::cout << "Client disconnected. Client FD: " << client_fd << std::endl;
                close(client_fd);
                break;
            } else {
                perror("Failed to read from fd");
            }
        }
    }
}

int main() {
    server srv(8080);  // Set server to listen on port 8080
    srv.start();
    return 0;
}