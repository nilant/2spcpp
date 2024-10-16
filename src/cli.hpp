#pragma  once
#include <filesystem>

namespace fs = std::filesystem;

struct Args {

    fs::path input_file;
    std::string exec_name;
    int iterlimit{0};
    int timelimit{0};
    int memlimit{0};
    int threads{0};
    int seed{0};

    Args(int argc, char* argv[]);
};