#include "coord.hpp"
#include "bottom_left.hpp"
#include "cli.hpp"
#include <gurobi_c++.h>
#include <fmt/core.h>

int main(int argc, char* argv[]) {

    Args args{argc, argv};

    GRBEnv env{};
    Instance inst{args.input_file};

    
    std::sort(inst.selected_items.begin(), inst.selected_items.end(), 
        [](auto const& a, auto const& b) {
            if ((a.h > b.h) || (a.h == b.h && a.w < b.w)) {
                return true;
            }
            return false;
        }
    );

    std::vector<Config> items(inst.selected_items);
    int ub = bottom_left_impl(items.begin(), items.end(), inst.w, inst.ub);
    inst.ub = ub;

    Coord coord{env, inst, ub};
    MIPResult res = coord.optimize(args, ub);

    return 0;
}