#ifndef LEADER_FOLLOWER_SERVER_HPP
#define LEADER_FOLLOWER_SERVER_HPP

#include "server.hpp"
#include "threadPool.hpp"
#include "pipelineData.hpp"
#include "task.hpp"
#include <atomic>
#include <netinet/in.h>
#include <string>
#include <sstream>

class LeaderFollowerServer : public server {
public:
    LeaderFollowerServer(int port, size_t numThreads);
    void start() override;
    void stop() override;

private:
    int port;
    std::atomic<bool> running;
    int server_fd;
    struct sockaddr_in address;
    threadPool pool;

    void handleClient(int client_fd, std::shared_ptr<pipelineData> data);
    void processCommand(const std::string& command, std::shared_ptr<pipelineData> data);
    void handleCreateCommand(std::istringstream& iss, std::shared_ptr<pipelineData> data);
    void handleAddCommand(std::istringstream& iss, std::shared_ptr<pipelineData> data);
    void handleSolveCommand(std::istringstream& iss, std::shared_ptr<pipelineData> data);
};

#endif // LEADER_FOLLOWER_SERVER_HPP