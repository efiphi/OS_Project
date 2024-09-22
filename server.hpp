#ifndef SERVER_HPP
#define SERVER_HPP

#include "ActiveObject.hpp"
#include "graph.hpp"
#include "threadPool.hpp"
#include "pipelineData.hpp"
#include <memory>

class server {
public:
    server(int port);
    void start();
    void stop();
    static server& getInstance() {
        static server instance(8080); // Example port
        return instance;
    }
    
    // Define ActiveObjects for each pipeline stage
    ActiveObject commandProcessing;
    ActiveObject graphUpdate;
    ActiveObject mstComputation;
    ActiveObject response;
    
private:
    int port;

    
   

    void handleClient(int client_fd, std::shared_ptr<pipelineData> data);
};

#endif // SERVER_HPP
