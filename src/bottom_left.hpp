#pragma once

#include "instance.hpp"
#include "heuresult.hpp"

HeurResult bottom_left(Instance const& inst);
int bottom_left_impl(std::vector<Config> const& items, int ub, int w);