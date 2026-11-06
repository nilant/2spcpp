// Entry point `bottom_left_plus`: run the bottom-left+ heuristic on an instance.
// Usage: bottom_left_plus <instance.json>  (result is added to the JSON file)

#include <fmt/core.h>

#include "bottom_left_plus.hpp"
#include "cli.hpp"

int main(int argc, char* argv[]) {

    Args args{argc, argv};

    Instance inst{args.input_file};

    HeurResult res = bottom_left_plus(inst);
    res.write(args.input_file);
    res.print();

    assert(res.obj > res.bound);

    return 0;
}