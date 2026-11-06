#pragma once

#include "instance.hpp"
#include "heuresult.hpp"

// Bottom-left heuristic: every task uses its first configuration. Gives a
// feasible packing height (obj) and a trivial area bound (bound).
HeurResult bottom_left(Instance const& inst);