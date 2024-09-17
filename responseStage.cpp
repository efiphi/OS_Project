#include "responseStage.hpp"
#include <iostream>
#include <unistd.h>

void responseStage::process(std::shared_ptr<pipelineData> data) {
    if (data && data->client_fd != -1) {
        std::cout << "Sending response to client (FD: " << data->client_fd << "): " << data->response << std::endl;
        
        ssize_t bytes_written = write(data->client_fd, data->response.c_str(), data->response.length());
        
        if (bytes_written == -1) {
            perror("Failed to write to client");
        } else {
            std::cout << "Response successfully written to Client FD: " << data->client_fd << std::endl;
        }
    } else {
        std::cerr << "Invalid pipeline data or client_fd. Response not sent." << std::endl;
    }
}
