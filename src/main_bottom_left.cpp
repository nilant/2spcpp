// Entry point `bottom_left`: run the bottom-left heuristic on an instance.
// Usage: bottom_left <instance.json>  (result is added to the JSON file)

#include <fmt/core.h>

#include "bottom_left.hpp"
#include "cli.hpp"

int main(int argc, char* argv[]) {

    Args args{argc, argv};

    Instance inst{args.input_file};

    HeurResult res = bottom_left(inst);
    res.write(args.input_file);
    res.print();

    assert(res.obj > res.bound);

    return 0;
}