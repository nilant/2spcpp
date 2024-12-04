#pragma  once
#include <filesystem>

namespace fs = std::filesystem;

struct Args {

    fs::path input_file;
    std::string exec_name;
    int iterlimit{0};
    double timelimit{0};
    double memlimit{0};
    int threads{0};
    int seed{0};

    //brkga
    unsigned p{0};
    double pe{0.0};
    double pm{0.0};
    double rhoe{0.0};
    unsigned K{0};

    Args(int argc, char* argv[]);
    void print() const;
};