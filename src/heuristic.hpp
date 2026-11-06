#pragma once

#include "cli.hpp"
#include "heuresult.hpp"
#include "instance.hpp"

#include <gurobi_c++.h>

// Matheuristic: shelves MIP + iterative pairing of shelves (see heuristic.cpp).
HeurResult heuristic(GRBEnv& env, Instance const& inst, Args const& args);