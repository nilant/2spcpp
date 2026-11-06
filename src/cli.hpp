#pragma  once
#include <filesystem>

namespace fs = std::filesystem;

// Command line arguments, shared by all executables:
//   <exe> <instance.json> [--timelimit s] [--iterlimit n] [--memlimit GB]
//                         [--threads n] [--seed n]
// Defaults (see cli.cpp): timelimit=3600, iterlimit=500, memlimit=16,
// threads=1, seed=0.
struct Args {

    fs::path input_file;  // instance JSON, also used as output file
    std::string exec_name;  // executable name, used as result key
    int iterlimit{0};  // iteration limit (not used by the solvers yet)
    int timelimit{0};  // seconds, passed to Gurobi
    int memlimit{0};  // GB, Gurobi soft memory limit
    int threads{0};  // Gurobi threads
    int seed{0};  // Gurobi seed

    Args(int argc, char* argv[]);
};