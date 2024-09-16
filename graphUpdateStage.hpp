#ifndef GRAPH_UPDATE_STAGE_HPP
#define GRAPH_UPDATE_STAGE_HPP

#include "pipelineStage.hpp"
#include "pipelineData.hpp"

class graphUpdateStage : public pipelineStage {
public:
    void process(std::shared_ptr<pipelineData> data) override;
};

#endif // GRAPH_UPDATE_STAGE_HPP
