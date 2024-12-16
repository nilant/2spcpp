#include "instance.hpp"
#include <algorithm>
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

void Instance::print_selected(fs::path const& file_path, std::string inst_name) {

    std::ifstream file(file_path);

    std::ofstream file_ss{file_path};
    nlohmann::ordered_json j;

    j["name"] = inst_name;
    j["rmax"] = rmax;
    j["wmax"] = wmax;
    j["W"] = w;
    j["seed"] = seed;
    j["alpha"] = alpha;
    j["ntasks"] = ntasks;
    j["nitems"] = nitems;
    j["nitems2"] = nitems2;
    j["reff"] = reff;
    j["lb"] = lb;
    j["ub"] = ub;
    std::vector<nlohmann::ordered_json> jtasks;
    for (auto& task : tasks) {
        if (task.id == -1) continue;
        nlohmann::ordered_json jtask;
        jtask["id"] = task.id;
        jtask["effort"] = task.effort;
        jtask["repeat"] = task.repeat;

        std::vector<nlohmann::ordered_json> jconfigs;
        for (auto& config : selected_items) {
            if (config.task_id != task.id) continue;
            nlohmann::ordered_json jconf;
            jconf["width"] = config.w;
            jconf["height"] = config.h;

            jconfigs.push_back(jconf);
        }

        std::sort(jconfigs.begin(), jconfigs.end());
        jconfigs.erase(std::unique(jconfigs.begin(), jconfigs.end()), jconfigs.end());
        jtask["configs"] = jconfigs;

        jtasks.push_back(jtask);
    }

    j["tasks"] = jtasks;

    file_ss << std::setprecision(2)  << std::setw(4) << std::fixed;
    nlohmann::ordered_json jj;
    jj["instance"] = j;
    file_ss << jj << std::endl;
}

void Instance::print(fs::path const& file_path, std::string inst_name) {

    std::ifstream file(file_path);

    std::ofstream file_ss{file_path};
    nlohmann::ordered_json j;

    j["name"] = inst_name;
    j["rmax"] = rmax;
    j["wmax"] = wmax;
    j["W"] = w;
    j["seed"] = seed;
    j["alpha"] = alpha;
    j["ntasks"] = ntasks;
    j["nitems"] = nitems;
    j["nitems2"] = nitems2;
    j["reff"] = reff;
    j["lb"] = lb;
    j["ub"] = ub;
    std::vector<nlohmann::ordered_json> jtasks;
    for (auto& task : tasks) {
        if (task.id == -1) continue;
        nlohmann::ordered_json jtask;
        jtask["id"] = task.id;
        jtask["effort"] = task.effort;
        jtask["repeat"] = task.repeat;

        std::vector<nlohmann::ordered_json> jconfigs;
        for (auto& config : task.configs) {
            nlohmann::ordered_json jconf;
            jconf["width"] = config.w;
            jconf["height"] = config.h;

            jconfigs.push_back(jconf);
        }

        jtask["configs"] = jconfigs;

        jtasks.push_back(jtask);
    }

    j["tasks"] = jtasks;

    file_ss << std::setprecision(2)  << std::setw(4) << std::fixed;
    nlohmann::ordered_json jj;
    jj["instance"] = j;
    file_ss << jj << std::endl;
}

void Instance::area() {
    int fill_area = 0;
    for (auto const& item : selected_items) {
        fill_area += item.h * item.w;
    }

    int total_area = ub * w;

    fill_ratio = static_cast<double>(fill_area) / total_area;
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

    auto items1{inst1.selected_items};
    std::sort(items1.begin(), items1.end(), [](auto const& a, auto const& b) {return a.h > b.h; });
    int p = 0;
    for (auto& item : items1) {
        inst.selected_items.push_back(item);
        inst.selected_items.back().y = 0;
        inst.selected_items.back().x = p;
        p += inst.selected_items.back().w;
    }

    auto items2{inst2.selected_items};
    std::sort(items2.begin(), items2.end(), [](auto const& a, auto const& b) {return a.h > b.h; });
    p = 0;
    for (auto& item : items2) {
        inst.selected_items.push_back(item);
        inst.selected_items.back().y = items1[0].h;
        inst.selected_items.back().x = p;
        p += inst.selected_items.back().w;
    }

    inst.area();
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

nlohmann::ordered_json Solution::to_json() {
    nlohmann::ordered_json jsol;

    std::vector<nlohmann::ordered_json> jitems;
    for (auto it = items.begin(); it != items.end(); ++it) {
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

    jsol["items"] = jitems;
    jsol["obj"] = obj;

    return jsol;
}

void Solution::write(fs::path const& file_path) {
     
    std::ofstream file_ss{file_path};
    file_ss << std::setprecision(2)  << std::setw(4) << std::fixed;

    file_ss << this->to_json() << std::endl;
}