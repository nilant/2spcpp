#include "instance.hpp"
#include <fstream>

Instance::Instance(fs::path const& input_file) {
    
    std::ifstream file(input_file);
    json j = json::parse(file)["instance"];
    rmax = j["rmax"];
    wmax = j["wmax"];
    w = j["W"];
    alpha = j["alpha"];
    seed = j["seed"];
    ntasks = j["ntasks"];
    nitems = j["nitems"];
    reff = j["reff"];
    lb = j["lb"];
    ub = j["ub"];

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