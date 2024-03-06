#include "coord2.hpp"
#include "mipresult.hpp"

int main(int argc, char* argv[]) {

    Args args{argc, argv};

    GRBEnv env{};
    Instance inst{args.input_file};
    Coord2 coord2{env, inst};
    MIPResult res = coord2.optimize(args, inst.ub);
    res.write(args.input_file);

    return 0;
}