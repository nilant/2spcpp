#include <fmt/core.h>
#include "cli.hpp"
#include "../libs/argh.h"

Args::Args(int argc, char* argv[]) {
    argh::parser cmdl({"--timelimit", "--memlimit", "--iterlimit",
                       "--threads", "--seed", "--p", "--pe", "--pm", "--rhoe", "--K"});
    cmdl.parse(argc, argv, argh::parser::SINGLE_DASH_IS_MULTIFLAG);

    input_file = fs::path(cmdl[1]);
    exec_name = std::string(fs::path(cmdl[0]).stem());
    cmdl("--timelimit", 600) >> timelimit;
    cmdl("--iterlimit", 1000) >> iterlimit;
    cmdl("--memlimit", 16) >> memlimit;
    cmdl("--threads", 1) >> threads;
    cmdl("--seed", 0) >> seed;
    cmdl("--p", 100) >> p;
    cmdl("--pe", 0.25) >> pe;
    cmdl("--pm", 0.1) >> pm;
    cmdl("--rhoe", 0.7) >> rhoe;
    cmdl("--K", 1) >> K;
}

void Args::print() const {
    fmt::print("timelimit: {}\niterlimit: {}\nmemlimit: {}\nthreads: {}\nseed: {}\n",timelimit, iterlimit, memlimit, threads, seed);
}