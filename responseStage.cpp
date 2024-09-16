#include "responseStage.hpp"
#include <iostream>

void responseStage::process(std::shared_ptr<pipelineData> data) {
    // Generate and send response to the client
    // In a real server, this would involve network communication
    std::cout << "Sending response to client: " << data->result << std::endl;
}
