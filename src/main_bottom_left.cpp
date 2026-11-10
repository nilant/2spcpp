// Entry point `bottom_left`: run the bottom-left heuristic on an instance.
// Usage: bottom_left <instance.json>  (prints the result as JSON)

#include <fmt/core.h>

#include "bottom_left.hpp"
#include "cli.hpp"

int main(int argc, char* argv[]) {

    Args args{argc, argv};

    Instance inst{args.input_file};
    fmt::print("##### Solving instance {}...\n", inst.name);

    HeurResult res = bottom_left(inst);
    res.print();

    assert(res.obj > res.bound);

    return 0;
}