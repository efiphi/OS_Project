#ifndef MST_FACTORY_HPP
#define MST_FACTORY_HPP

#include "mst_solver.hpp"
#include "prim_mst_solver.hpp"
#include "kruskal_mst_solver.hpp"

enum MSTAlgorithmType {
    PRIM,
    KRUSKAL
};

class MSTFactory {
public:
    static MSTSolver* createSolver(MSTAlgorithmType type) {
        if (type == PRIM) {
            return new PrimMSTSolver();
        } else if (type == KRUSKAL) {
            return new KruskalMSTSolver();
        }
        return nullptr;
    }
};

#endif // MST_FACTORY_HPP
