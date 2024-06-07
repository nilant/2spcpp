#include "instance.hpp"
#include <fstream>
#include <vector>
#include <fmt/core.h>

Instance::Instance(fs::path const& input_file) {
    
    std::ifstream file(input_file);
    json j = json::parse(file)["instance"];
    name = j["name"];
    rmax = j["rmax"];
    wmax = j["wmax"];
    w = j["W"];
    alpha = j["alpha"];
    seed = j["seed"];
    ntasks = j["ntasks"];
    nitems = j["nitems"];
    reff = j["reff"];
    lb = j["lb"];

    nitems2 = 0;

    int id = 0;
    items.reserve(nitems);
    tasks.reserve(j["tasks"].size());
    for (auto const& jtask : j["tasks"]) {
        Task task;
        task.id = jtask["id"];
        task.effort = jtask["effort"];
        task.repeat = jtask["repeat"];
        int nconfigs = jtask["configs"].size();
        task.configs.reserve(nconfigs);

        nitems2 += task.repeat * nconfigs;

        for (auto const& jconfig : jtask["configs"]) {
            Config config;
            config.repeat = task.repeat;
            config.id = id;
            config.task_id = task.id;
            config.w = jconfig["width"];
            config.h = jconfig["height"];
            task.configs.push_back(config);
            items.push_back(config);
            id++;
        }
        tasks.push_back(task);
    }

    for (auto const& task : tasks) {
        for (int r = 0; r < task.repeat; ++r) {
            selected_items.push_back(task.configs[0]);
        }
    }
    
    ub = 0;
    for (auto const& task : tasks) {
        ub += task.configs[0].h;
    }
}

Instance merge(Instance const& inst1, Instance const& inst2) {

    Instance inst{};

    inst.name = inst1.name;
    inst.rmax = inst1.rmax;
    inst.wmax = inst1.wmax;
    inst.w = inst1.w;
    inst.seed = inst1.seed;
    inst.alpha = inst1.alpha;
    inst.reff = inst1.reff + inst2.reff;
    inst.ub = inst1.ub + inst2.ub;

    inst.ntasks = inst1.ntasks;
    inst.nitems = inst1.nitems;

    inst.tasks.resize(inst.ntasks);

    for (auto const& task : inst1.tasks) {
        if (task.id == -1) continue;
        inst.tasks[task.id] = task;
    }

    for (auto& task : inst2.tasks) {
        if (task.id == -1) continue;
        if (inst.tasks[task.id].id == -1) {
            inst.tasks[task.id] = task;
        } else { // already present
            inst.tasks[task.id].repeat += task.repeat;
            for (auto& item : inst.tasks[task.id].configs) {
                item.repeat = inst.tasks[task.id].repeat;
            }
        }
    }

    for (auto const& task : inst.tasks) {
        if (task.id == -1) continue;
        for (auto const& item : task.configs) {
            inst.items.push_back(item);
        }
    }

    for (auto const& item : inst1.selected_items) {
        inst.selected_items.push_back(item);
    }

    for (auto const& item : inst2.selected_items) {
        inst.selected_items.push_back(item);
    }

    return inst;
}

inline bool overlap_1d(int x1_min, int x1_max, int x2_min, int x2_max) {
    return x1_max > x2_min && x2_max > x1_min;
}

bool check_feas(std::vector<Config> const& items) {
    for (int i = 0; i < items.size()-1; ++i) {
        for (int j = i+1; j <items.size(); ++j) {
            auto const& item1 = items[i];
            auto const& item2 = items[j];
            if (overlap_1d(item1.x, item1.x + item1.w, item2.x, item2.x + item2.w) &&
                overlap_1d(item1.y, item1.y + item1.h, item2.y, item2.y + item2.h)) {
                    fmt::print("item {}(({},{})-({},{})) and item {}(({},{})-({},{})) overlaps\n", item1.id, item1.x, item1.y, item1.w, item1.h, item2.id, item2.x, item2.y, item2.w, item2.h);
                    return false;
                }
        }
    }
    return true;
}

void print_solution(std::vector<Config>::iterator begin, std::vector<Config>::iterator end, int obj) {
    std::ofstream file_ss{"data/solution.json"};
    file_ss << std::setprecision(2)  << std::setw(4) << std::fixed;

    nlohmann::ordered_json jsol;

    std::vector<nlohmann::ordered_json> jitems;
    for (auto it = begin; it != end; ++it) {
        auto const& item = *it;
        nlohmann::ordered_json j;
        j["id"] = item.id;
        j["task_id"] = item.task_id;
        j["repeat"] = item.repeat;
        j["w"] = item.w;
        j["h"] = item.h;
        j["x"] = item.x;
        j["y"] = item.y;

        jitems.push_back(j);
    }

    jsol["solution"] = jitems;
    jsol["obj"] = obj;

    file_ss << jsol << std::endl;
}