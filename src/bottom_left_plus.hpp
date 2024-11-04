#pragma once

#include "instance.hpp"
#include "heuresult.hpp"

int bottom_left_impl(std::vector<Config>::iterator begin, std::vector<Config>::iterator end, int w, int ub);
HeurResult bottom_left(Instance& inst);