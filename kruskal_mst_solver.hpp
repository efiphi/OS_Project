#ifndef KRUSKAL_MST_SOLVER_HPP
#define KRUSKAL_MST_SOLVER_HPP

#include "mst_solver.hpp"
#include <vector>

class KruskalMSTSolver : public MSTSolver {
public:
    std::vector<Edge> solveMST(Graph& graph) override; // Change return type to std::vector<Edge>
    std::string getMSTResults(Graph& graph, const std::vector<Edge>& mstEdges) override;
};

#endif // KRUSKAL_MST_SOLVER_HPP