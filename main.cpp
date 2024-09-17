#include "server.hpp"

int main() {
    // Define the port number for the server to listen on
    int port = 12346;

    // Create and start the server
    server mstServer(port);
    mstServer.start();

    return 0;
}
