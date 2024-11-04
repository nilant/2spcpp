#pragma once

#include "cli.hpp"
#include "heuresult.hpp"
#include "instance.hpp"

#include <gurobi_c++.h>

HeurResult heuristic(GRBEnv& env, Instance const& inst, Args const& args);