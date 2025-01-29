#include "coord.hpp"
#include "bottom_left.hpp"
#include "cli.hpp"
#include <gurobi_c++.h>
#include <fmt/core.h>

int main(int argc, char* argv[]) {

    try {

        Args args{argc, argv};

        GRBEnv env{};
        Instance inst{args.input_file};

        auto res_bl = bottom_left(inst);
        int ub = res_bl.obj;

        Coord coord{env, inst, ub};
        MIPResult res = coord.optimize(args, ub);
        res.write(args.input_file);
        res.print();

    } catch (GRBException& e) {
        fmt::print("error code={}\n", e.getErrorCode());
        fmt::print("error message={}\n", e.getMessage());
    }
    
    return 0;
}
