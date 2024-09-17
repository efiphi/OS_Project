#ifndef MST_FACTORY_HPP
#define MST_FACTORY_HPP

#include "mst_solver.hpp"
#include "prim_mst_solver.hpp"
#include "kruskal_mst_solver.hpp"
#include <memory>

enum MSTAlgorithmType {
    PRIM,
    KRUSKAL
};

class MSTFactory {
public:
    static std::unique_ptr<MSTSolver> createSolver(MSTAlgorithmType type) {
        if (type == PRIM) {
            return std::make_unique<PrimMSTSolver>();
        } else if (type == KRUSKAL) {
            return std::make_unique<KruskalMSTSolver>();
        }
        return nullptr;
    }
};
#endif // MST_FACTORY_HPP
