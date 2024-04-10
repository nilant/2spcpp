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
        res.write(args.input_file);
        res.print();

    } catch (GRBException& e) {
        fmt::print("error code={}\n", e.getErrorCode());
        fmt::print("error message={}\n", e.getMessage());
    }

    return 0;
}