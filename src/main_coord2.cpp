#include <fmt/core.h>

#include "bottom_left_plus.hpp"
#include "coord2.hpp"
#include "mipresult.hpp"

int main(int argc, char* argv[]) {

    try {
        Args args{argc, argv};

        GRBEnv env{};
        Instance inst{args.input_file};

        auto res_bl = bottom_left_plus(inst);
        int ub = res_bl.obj;
        
        Coord2 coord2{env, inst, ub};
        MIPResult res = coord2.optimize(args);
        res.write(args.input_file);
        res.print();

    } catch (GRBException& e) {
        fmt::print("error code={}\n", e.getErrorCode());
        fmt::print("error message={}\n", e.getMessage());
    }

    return 0;
}