#ifndef RESPONSE_STAGE_HPP
#define RESPONSE_STAGE_HPP

#include "pipelineStage.hpp"
#include "pipelineData.hpp"

class responseStage : public pipelineStage {
public:
    void process(std::shared_ptr<pipelineData> data) override;
};

#endif // RESPONSE_STAGE_HPP
