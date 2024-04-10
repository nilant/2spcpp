#include <fmt/core.h>

#include "bottom_left.hpp"
#include "coord.hpp"
#include "mipresult.hpp"

int main(int argc, char* argv[]) {

    try {

        Args args{argc, argv};

        GRBEnv env{};
        Instance inst{args.input_file};

        auto res_bl = bottom_left(inst);
        int ub = res_bl.obj;

        Coord coord{env, inst, ub};
        MIPResult res = coord.optimize(args);
        res.write(args.input_file);
        res.print();

    } catch (GRBException& e) {
        fmt::print("error code={}", e.getErrorCode());
        fmt::print("error message={}", e.getMessage());
    }

    return 0;
}