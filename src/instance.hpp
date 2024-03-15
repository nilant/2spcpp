#pragma once 

#include "../libs/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

struct Config {
    int id;
    int task_id;
    int repeat;
    int w;
    int h;
};

struct Task {
    int id;
    int effort;
    int repeat;
    std::vector<Config> configs; 
};

struct Instance {
    std::string name;
    int rmax;
    int wmax;
    int w;
    int seed;
    int alpha;
    int ntasks;
    int nitems;
    int nitems2; // for coord2
    int reff;
    int lb;
    std::vector<Task> tasks;
    std::vector<Config> items;

    Instance(fs::path const& dat_file);
};
