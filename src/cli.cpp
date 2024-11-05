#include "cli.hpp"
#include "../libs/argh.h"

Args::Args(int argc, char* argv[]) {
    argh::parser cmdl({"--timelimit", "--memlimit", "--iterlimit",
                       "--threads", "--seed"});
    cmdl.parse(argc, argv, argh::parser::SINGLE_DASH_IS_MULTIFLAG);

    input_file = fs::path(cmdl[1]);
    exec_name = std::string(fs::path(cmdl[0]).stem());
    cmdl("--timelimit", 3600) >> timelimit;
    cmdl("--iterlimit", 500) >> iterlimit;
    cmdl("--memlimit", 16) >> memlimit;
    cmdl("--threads", 1) >> threads;
    cmdl("--seed", 0) >> seed;
}