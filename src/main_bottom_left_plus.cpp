// Entry point `bottom_left_plus`: run the bottom-left+ heuristic on an instance.
// Usage: bottom_left_plus <instance.json>  (prints the result as JSON)

#include <fmt/core.h>

#include "bottom_left_plus.hpp"
#include "cli.hpp"

int main(int argc, char* argv[]) {

    Args args{argc, argv};

    Instance inst{args.input_file};

    HeurResult res = bottom_left_plus(inst);
    res.print();

    assert(res.obj > res.bound);

    return 0;
}