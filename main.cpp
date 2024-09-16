#include "server.hpp"

int main() {
    // Define the port number for the server to listen on
    int port = 12345;
    size_t threadCount = 4; // Define the number of threads for the thread pool

    // Create and start the server
    server mstServer(port, threadCount);
    mstServer.start();

    return 0;
}
