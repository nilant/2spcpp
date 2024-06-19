#pragma once 

#include "../libs/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

struct Config {
    int id{-1};
    int task_id{-1};
    int repeat{-1};
    int w{-1};
    int h{-1};
    int x{-1};
    int y{-1};
};

struct Task {
    int id{-1};
    int effort{-1};
    int repeat{-1};
    std::vector<Config> configs; 
};

struct Instance {
    std::string name;
    int rmax{0};
    int wmax{0};
    int w{0};
    int seed{0};
    int alpha{0};
    int ntasks{0};
    int nitems{0};
    int nitems2{0}; // for coord2
    int reff{0};
    int lb{0};
    int ub{0};
    std::vector<Task> tasks;
    std::vector<Config> items;
    std::vector<Config> selected_items;

    Instance() {};
    Instance(fs::path const& dat_file);
    void print(fs::path const& dat_file, std::string name);
};

Instance merge(Instance const& inst1, Instance const& inst2);
bool check_feas(std::vector<Config> const& items);
void print_solution(std::vector<Config>::iterator begin, std::vector<Config>::iterator end, int obj);