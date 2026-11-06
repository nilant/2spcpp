#pragma once 

#include "../libs/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

// A configuration of a task: a rectangle w x h (w = #CPUs, h = running time).
// Once placed in the strip, (x, y) is its bottom-left corner (-1 = unplaced).
// `id` is a global item id, `task_id` the owning task, `repeat` the number of
// repetitions of the owning task.
struct Config {
    int id{-1};
    int task_id{-1};
    int repeat{-1};
    int w{-1};
    int h{-1};
    int x{-1};
    int y{-1};
};

// A task: executed `repeat` times, each one with one of its `configs`.
// `effort` is the sequential work used to generate the configurations.
struct Task {
    int id{-1};
    int effort{-1};
    int repeat{-1};
    std::vector<Config> configs; 
};

// A problem instance (loaded from JSON, see README for the format). The
// strip has width `w`; the goal is to place all task repetitions, choosing one
// configuration each, minimizing the height of the strip.
struct Instance {
    std::string name;
    int rmax{0};  // maximum repetitions of a task
    int wmax{0};  // maximum CPUs assignable to a task
    int w{0};  // strip width (number of CPUs)
    int seed{0};  // generator seed label
    int alpha{0};  // parallel percentage (Amdahl), 0..100
    int ntasks{0};  // number of tasks
    int nitems{0};  // number of configurations over all tasks
    int nitems2{0}; // for coord2
    int reff{0};  // total repetitions (sum of task.repeat)
    int lb{0};  // lower bound on the height
    int ub{0};  // upper bound / current height of the solution
    double fill_ratio{0.0};  // used area / (ub * w), see area()
    std::vector<Task> tasks;
    std::vector<Config> items;
    std::vector<Config> selected_items;  // chosen items, one per repetition

    Instance() {};
    // Load an instance from the "instance" object of a JSON file.
    Instance(fs::path const& dat_file);
    // Recompute fill_ratio from selected_items.
    void area();
    // Write the instance (all configurations) as JSON to `dat_file`.
    void print(fs::path const& dat_file, std::string name);
    // Like print(), but only writes the configurations in selected_items.
    void print_selected(fs::path const& dat_file, std::string name);
};

// A packing: placed items and the height (obj) of the strip.
struct Solution {
    int obj;
    std::string name;
    std::vector<Config> items;
    
    nlohmann::ordered_json to_json();
    void write(fs::path const& file_path);
};

// Merge two sub-instances: the items of inst2 are stacked on top of those of
// inst1 (each side laid out as one shelf). ub is the sum of the two heights.
Instance merge(Instance const& inst1, Instance const& inst2);
// True if no two items overlap (prints the first overlapping pair).
bool check_feas(std::vector<Config> const& items);