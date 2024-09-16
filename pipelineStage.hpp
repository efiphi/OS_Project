#ifndef PIPELINE_STAGE_HPP
#define PIPELINE_STAGE_HPP

#include <memory>


class pipelineData;

class pipelineStage {
public:
    virtual ~pipelineStage() {}
    virtual void process(std::shared_ptr<pipelineData> data) = 0;
    void setNextStage(std::shared_ptr<pipelineStage> next) { nextStage = next; }

protected:
    std::shared_ptr<pipelineStage> nextStage;
};

#endif // PIPELINE_STAGE_HPP
