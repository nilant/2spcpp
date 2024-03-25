#include "instance.hpp"
#include <fstream>

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
        inst.tasks[task.id] = task;
    }

    for (auto& task : inst2.tasks) {
        if (inst.tasks[task.id].id == -1) {
            inst.tasks[task.id] = task;
        } else { // already present
            inst.tasks[task.id].repeat += task.repeat;
            for (auto& item : inst.tasks[task.id].configs) {
                item.repeat = inst.tasks[task.id].repeat;
            }
        }
    }

    auto it = std::remove_if(inst.tasks.begin(), inst.tasks.end(), [](auto const& task) { return task.id == -1; });
    inst.tasks.erase(it, inst.tasks.end());

    for (auto const& task : inst.tasks) {
        for (auto const& item : task.configs) {
            inst.items.push_back(item);
        }
    }

    return inst;
}