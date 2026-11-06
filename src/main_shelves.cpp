// Entry point `shelves`: solve the shelf-based MIP with Gurobi.
// Usage: shelves <instance.json> [--timelimit s ...]

#include <fmt/core.h>

#include "gurobi_c++.h"
#include "shelves.hpp"
#include "mipresult.hpp"

int main(int argc, char* argv[]) {

    try {

        Args args{argc, argv};

        GRBEnv env{};
        Instance inst{args.input_file};
        Shelves shelves{env, inst};
        MIPResult res = shelves.optimize(args);
        res.print();

    } catch (GRBException& e) {
        fmt::print(stderr, "error code={}\n", e.getErrorCode());
        fmt::print(stderr, "error message={}\n", e.getMessage());
    }

    return 0;
}