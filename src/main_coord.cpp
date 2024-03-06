#include "coord.hpp"
#include "mipresult.hpp"

int main(int argc, char* argv[]) {

    Args args{argc, argv};

    GRBEnv env{};
    Instance inst{args.input_file};
    Coord coord{env, inst};
    MIPResult res = coord.optimize(args, inst.ub);
    res.write(args.input_file);

    return 0;
}