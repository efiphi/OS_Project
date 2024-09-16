#ifndef SERVER_HPP
#define SERVER_HPP

#include "threadPool.hpp" 
#include <netinet/in.h>   // Include for sockaddr_in

class server {
public:
    server(int port, size_t threadCount);
    void start();

private:
    int port;
    threadPool pool; 

    static void handleClient(int client_fd);
};

#endif // SERVER_HPP
