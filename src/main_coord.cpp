#include "coord.hpp"
#include "mipresult.hpp"
#include "bottom_left.hpp"

int main(int argc, char* argv[]) {

    Args args{argc, argv};

    GRBEnv env{};
    Instance inst{args.input_file};

	auto bl_res = bottom_left(inst);
	inst.ub = bl_res.obj;

    Coord coord{env, inst};
    MIPResult res = coord.optimize(args, inst.ub);
    res.write(args.input_file);

    return 0;
}