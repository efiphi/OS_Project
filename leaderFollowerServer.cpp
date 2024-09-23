#include "leaderFollowerServer.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstring>


LeaderFollowerServer::LeaderFollowerServer(int port, size_t numThreads) 
    : server(port), running(false), pool(numThreads) {}

void LeaderFollowerServer::start() {
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Leader-Follower Server listening on port " << port << std::endl;

    running = true;
    pool.start();

    while(running) {
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        std::cout << "New connection accepted" << std::endl;

        auto data = std::make_shared<pipelineData>();
        data->client_fd = new_socket;

        pool.enqueue(std::make_shared<task>([this, new_socket, data]() {
            this->handleClient(new_socket, data);
        }));
    }
}

void LeaderFollowerServer::stop() {
    running = false;
    pool.stop();
    close(server_fd);
}

void LeaderFollowerServer::handleClient(int client_fd, std::shared_ptr<pipelineData> data) {
    char buffer[1024] = {0};
    int valread = read(client_fd, buffer, 1024);
    if (valread > 0) {
        std::string received(buffer);
        processCommand(received, data);
    }
}

void LeaderFollowerServer::processCommand(const std::string& command, std::shared_ptr<pipelineData> data) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    try {
        if (cmd == "create") {
            handleCreateCommand(iss, data);
        } else if (cmd == "add") {
            handleAddCommand(iss, data);
        } else if (cmd == "solve") {
            handleSolveCommand(iss, data);
        } else {
            data->response = "Unknown command.";
            task::enqueueTask(TaskType::Response, data);
        }
    } catch (const std::exception& e) {
        data->response = "Error processing command: " + std::string(e.what());
        task::enqueueTask(TaskType::Response, data);
    }
}

void LeaderFollowerServer::handleCreateCommand(std::istringstream& iss, std::shared_ptr<pipelineData> data) {
    int V, E;
    if (iss >> V >> E) {
        data->graph = Graph(V);
        data->vertices = V;
        data->edges = E;
        data->response = "Graph created with " + std::to_string(V) + " vertices and " + std::to_string(E) + " edges.\n";
        std::cout << "Debug: Enqueuing Response task for 'create'" << std::endl;
        task::enqueueTask(TaskType::Response, data);
    } else {
        std::string response = "Invalid input for create command. Usage: create <vertices> <edges>\n";
        write(data->client_fd, response.c_str(), response.length());
    }
}

void LeaderFollowerServer::handleAddCommand(std::istringstream& iss, std::shared_ptr<pipelineData> data) {
    int v, w, weight;
    if (iss >> v >> w >> weight) {
        data->v = v;
        data->w = w;
        data->weight = weight;
        std::cout << "Debug: Enqueuing GraphUpdate task for 'add'" << std::endl;
        task::enqueueTask(TaskType::GraphUpdate, data);
    } else {
        data->response = "Invalid input for add command.";
        task::enqueueTask(TaskType::Response, data);
    }
}

void LeaderFollowerServer::handleSolveCommand(std::istringstream& iss, std::shared_ptr<pipelineData> data) {
    std::string algo;
    if (iss >> algo) {
        data->algorithm = algo;
        std::cout << "Debug: Enqueuing MSTComputation task for 'solve'" << std::endl;
        task::enqueueTask(TaskType::MSTComputation, data);
    } else {
        data->response = "Invalid input for solve command.";
        task::enqueueTask(TaskType::Response, data);
    }
}