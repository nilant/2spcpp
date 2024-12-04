#include <fmt/core.h>

#include "heuresult.hpp"
#include "heuristic.hpp"


int main(int argc, char* argv[]) {

    Args args{argc, argv};

    Instance inst{args.input_file};

    HeurResult res = heuristic(inst, args);
    res.write(args.input_file);
    res.print();

    return 0;
}
