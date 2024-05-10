#include <fmt/core.h>

#include "bottom_left.hpp"
#include "cli.hpp"

int main(int argc, char* argv[]) {

    Args args{argc, argv};

    Instance inst{args.input_file};

    inst.selected_items.reserve(inst.ntasks * inst.rmax);
    for (auto const& task : inst.tasks) {
        for (int r = 0; r < task.repeat; ++r) {
            inst.selected_items.push_back(task.configs[0]);
        }
    }

    HeurResult res = bottom_left(inst);
    res.write(args.input_file);
    res.print();

    assert(res.obj > res.bound);

    return 0;
}