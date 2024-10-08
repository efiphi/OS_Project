#include "server.hpp"
#include <csignal>
#include <iostream>

// Server instance to use in signal handler
server* globalServerInstance = nullptr;

// Signal handler function to handle Ctrl+C
void signalHandler(int signum) {
    std::cout << "\nInterrupt signal (" << signum << ") received. Shutting down server..." << std::endl;
    if (globalServerInstance) {
        globalServerInstance->stop(); // Assuming you have a stop method in your server
    }
    exit(signum);
}

int main() {
    // Register signal handler
    signal(SIGINT, signalHandler);

    server srv(12346);
    globalServerInstance = &srv;
    srv.start();

    return 0;
}
