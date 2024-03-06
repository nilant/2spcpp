#include "shelves.hpp"
#include "mipresult.hpp"

int main(int argc, char* argv[]) {

    Args args{argc, argv};

    GRBEnv env{};
    Instance inst{args.input_file};
    Shelves shelves{env, inst};
    MIPResult res = shelves.optimize(args, inst.ub);
    res.write(args.input_file);

    return 0;
}