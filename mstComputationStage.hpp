#ifndef MST_COMPUTATION_STAGE_HPP
#define MST_COMPUTATION_STAGE_HPP

#include "pipelineStage.hpp"
#include "pipelineData.hpp"
#include "mst_factory.hpp"  // Assuming this is where the MST algorithms are

class mstComputationStage : public pipelineStage {
public:
    void process(std::shared_ptr<pipelineData> data) override;
};

#endif // MST_COMPUTATION_STAGE_HPP
