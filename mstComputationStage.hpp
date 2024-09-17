#ifndef MST_COMPUTATION_STAGE_HPP
#define MST_COMPUTATION_STAGE_HPP

#include "pipelineStage.hpp"
#include "pipelineData.hpp"
#include "mst_factory.hpp"
#include <iostream>

class mstComputationStage : public pipelineStage {
public:
    void process(std::shared_ptr<pipelineData> data) override {
        if (!data) return;

        // Determine the algorithm to use
        MSTSolver* solver = MSTFactory::createSolver(data->algorithm == "prim" ? PRIM : KRUSKAL);
        if (solver) {
            // Compute the MST
            auto mstEdges = solver->solveMST(data->graph);
            
            // Generate the MST result string
            data->response = solver->getMSTResults(data->graph, mstEdges);

            // Debug output
            std::cout << "Debug: MST computed using " << data->algorithm << " algorithm." << std::endl;

            // Clean up the solver
            delete solver;
        } else {
            data->response = "Invalid algorithm specified.";
            std::cerr << "Error: Invalid algorithm specified for MST computation." << std::endl;
        }

        // Enqueue the response stage to send results to the client
        if (nextStage) {
            nextStage->process(data);
        } else {
            // If no next stage, enqueue the Response task
            task::enqueueTask(TaskType::Response, data);
        }
    }

    ~mstComputationStage() override = default; // Define a virtual destructor
};

#endif // MST_COMPUTATION_STAGE_HPP
