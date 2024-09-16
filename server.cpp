#include "server.hpp"
#include "graph.hpp"
#include "mst_factory.hpp"
#include "task.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

server::server(int port, size_t threadCount) : port(port), pool(threadCount) {}

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

        pool.enqueue(std::make_shared<task>([client_fd]() {
            handleClient(client_fd);
        }));
    }

    close(server_fd);
}

void server::handleClient(int client_fd) {
    char buffer[1024];
    Graph graph(0); // Initial empty graph
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

            try {
                if (cmd == "create") {
                    int V, E;
                    if (iss >> V >> E) {
                        graph = Graph(V); // Create a new graph
                        std::string response = "Graph created with " + std::to_string(V) + " vertices and " + std::to_string(E) + " edges.\n";
                        write(client_fd, response.c_str(), response.length());
                    } else {
                        std::string response = "Invalid input for create command. Usage: create <number_of_vertices> <number_of_edges>\n";
                        write(client_fd, response.c_str(), response.length());
                    }
                } else if (cmd == "add") {
                    int v, w, weight;
                    if (iss >> v >> w >> weight) {
                        graph.addEdge(v, w, weight);
                        std::string response = "Edge added from " + std::to_string(v) + " to " + std::to_string(w) + " with weight " + std::to_string(weight) + ".\n";
                        write(client_fd, response.c_str(), response.length());
                    } else {
                        std::string response = "Invalid input for add command. Usage: add <from_vertex> <to_vertex> <weight>\n";
                        write(client_fd, response.c_str(), response.length());
                    }
                } else if (cmd == "solve") {
                    std::string algo;
                    if (iss >> algo) {
                        MSTSolver* solver = nullptr;
                        if (algo == "prim") {
                            solver = MSTFactory::createSolver(PRIM);
                        } else if (algo == "kruskal") {
                            solver = MSTFactory::createSolver(KRUSKAL);
                        }

                       if (solver) {
                            std::vector<Edge> mstEdges = solver->solveMST(graph);
                            std::string mstResults = solver->getMSTResults(graph, mstEdges);
                             delete solver;
                             write(client_fd, mstResults.c_str(), mstResults.length()); // Send results to client
                                
                    } else {
                            std::string response = "Unknown algorithm. Use 'prim' or 'kruskal'.\n";
                            write(client_fd, response.c_str(), response.length());
                        }
                    } else {
                        std::string response = "Invalid input for solve command. Usage: solve <algorithm>\n";
                        write(client_fd, response.c_str(), response.length());
                    }
                } else {
                    std::string response = "Unknown command: " + cmd + "\n";
                    write(client_fd, response.c_str(), response.length());
                }
            } catch (const std::exception& e) {
                std::string response = "Error: " + std::string(e.what()) + "\n";
                write(client_fd, response.c_str(), response.length());
            }
        } else {
            if (bytes_read == 0) {
                std::cout << "Client disconnected" << std::endl;
                close(client_fd);
                break;
            } else {
                perror("Failed to read from fd");
            }
        }
    }
}
