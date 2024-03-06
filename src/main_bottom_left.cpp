#include "bottom_left.hpp"
#include "mipresult.hpp"

int main(int argc, char* argv[]) {

    Args args{argc, argv};

    GRBEnv env{};
    Instance inst{args.input_file};
    MIPResult res = bottom_left(inst);
    res.write(args.input_file);
    res.print();

    return 0;
}