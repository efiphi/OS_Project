#ifndef MST_SOLVER_HPP
#define MST_SOLVER_HPP

#include "graph.hpp"
#include <vector>
#include <string>

class MSTSolver {
public:
    virtual ~MSTSolver() = default; 
    virtual std::vector<Edge> solveMST(Graph& graph) = 0; // Pure virtual function
    virtual std::string getMSTResults(Graph& graph, const std::vector<Edge>& mstEdges); 
};

#endif // MST_SOLVER_HPP
