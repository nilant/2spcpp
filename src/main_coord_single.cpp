#include <fmt/core.h>

#include "bottom_left.hpp"
#include "coord.hpp"
#include "mipresult.hpp"

int main(int argc, char* argv[]) {

    try {

        Args args{argc, argv};

        GRBEnv env{};
        Instance inst{args.input_file};

        for (auto& task : inst.tasks) {
            task.configs.erase(task.configs.begin()+1, task.configs.end());
        }

        inst.items.clear();
        for (auto& task : inst.tasks) {
            for (auto& item : task.configs) {
                inst.items.push_back(item);
            }
        }

        auto res_bl = bottom_left(inst);
        int ub = res_bl.obj;

        Coord coord{env, inst, ub};
        MIPResult res = coord.optimize(args);
        res.name = "coord_single";
        res.write(args.input_file);
        res.print();

    } catch (GRBException& e) {
        fmt::print("error code={}\n", e.getErrorCode());
        fmt::print("error message={}\n", e.getMessage());
    }

    return 0;
}