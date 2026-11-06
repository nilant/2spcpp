#pragma once

#include "instance.hpp"
#include "heuresult.hpp"

// Core of bottom-left+: packs the items in [begin, end) in a strip of width w
// and maximum height ub, sets x/y on each item and returns the used height.
int bottom_left_plus_impl(std::vector<Config>::iterator begin, std::vector<Config>::iterator end, int w, int ub);
// Bottom-left+ on inst.selected_items (sorted in place). Upper bound for the
// search is 2*inst.ub. Positions are returned in HeurResult::sol.
HeurResult bottom_left_plus(Instance& inst);