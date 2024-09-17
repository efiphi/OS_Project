#include "server.hpp"
#include "graph.hpp"
#include "mst_factory.hpp"
#include "task.hpp"
#include "pipelineData.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

threadPool server::pool(4);

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

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
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
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        std::cout << "Accepted client connection. Client FD: " << client_fd << std::endl;

        // Allocate a new pipelineData for this client
        auto data = std::make_shared<pipelineData>();
        data->client_fd = client_fd;

        // Directly call handleClient for command processing
        handleClient(client_fd, data);
    }

    close(server_fd);
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
            data->command = cmd; // Store command in pipelineData

            try {
                if (cmd == "create") {
                    int V, E;
                    if (iss >> V >> E) {
                        data->graph = Graph(V);
                        data->vertices = V;
                        data->edges = E;
                        data->response = "Graph created with " + std::to_string(V) + " vertices and " + std::to_string(E) + " edges.\n";
                        std::cout << "Debug: Enqueuing Response task for 'create'" << std::endl;
                        task::enqueueTask(TaskType::Response, data);
                    } else {
                        std::string response = "Invalid input for create command. Usage: create <number_of_vertices> <number_of_edges>\n";
                        write(client_fd, response.c_str(), response.length());
                    }
                } else if (cmd == "add") {
                    int v, w, weight;
                    if (iss >> v >> w >> weight) {
                        data->v = v;
                        data->w = w;
                        data->weight = weight;
                        std::cout << "Debug: Enqueuing GraphUpdate task for 'add'" << std::endl;
                        // Enqueue to GraphUpdate stage
                        task::enqueueTask(TaskType::GraphUpdate, data);
                    } else {
                        data->response = "Invalid input for add command.";
                        task::enqueueTask(TaskType::Response, data);
                    }
                } else if (cmd == "solve") {
                    std::string algo;
                    if (iss >> algo) {
                        data->algorithm = algo;
                        // Enqueue to MSTComputation stage
                        std::cout << "Debug: Enqueuing MSTComputation task for 'solve'" << std::endl;
                        task::enqueueTask(TaskType::MSTComputation, data);
                    } else {
                        data->response = "Invalid input for solve command.";
                        task::enqueueTask(TaskType::Response, data);
                    }
                } else {
                    data->response = "Unknown command.";
                    task::enqueueTask(TaskType::Response, data);
                }
            } catch (const std::exception& e) {
                data->response = "Error: " + std::string(e.what());
                task::enqueueTask(TaskType::Response, data);
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
