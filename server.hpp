#ifndef SERVER_HPP
#define SERVER_HPP

#include "pipelineData.hpp"
#include "threadPool.hpp"
#include <memory>

class server {
public:
    // Constructor to initialize the server with the port number
    server(int port);

    // Start the server to listen and accept client connections
    void start();

    void stop();

    // Static thread pool accessible by tasks
    static threadPool pool;

private:
    int port;

    // Handle the client connection and enqueue tasks
    static void handleClient(int client_fd, std::shared_ptr<pipelineData> data);
};

#endif // SERVER_HPP
