#include "bottom_left.hpp"
#include "coord2.hpp"
#include "mipresult.hpp"

int main(int argc, char* argv[]) {

    Args args{argc, argv};

    GRBEnv env{};
    Instance inst{args.input_file};

    auto res_bl = bottom_left(inst);
    int ub = res_bl.obj;
    
    Coord2 coord2{env, inst, ub};
    MIPResult res = coord2.optimize(args);
    res.write(args.input_file);
    res.print();

    return 0;
}