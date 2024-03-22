#include "heuresult.hpp"
#include "heuristic.hpp"


int main(int argc, char* argv[]) {

    Args args{argc, argv};

    GRBEnv env{};
    Instance inst{args.input_file};

    HeurResult res = heuristic(env, inst, args);
    res.write(args.input_file);
    res.print();

    return 0;
}