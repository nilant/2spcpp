#include "heuresult.hpp"

#include <fmt/core.h>

#include "../libs/json.hpp"

void HeurResult::print() {
    fmt::print("{{\"name\":{},\"obj\":{:.2f},\"bound\":{:.2f},\"runtime\":{:.2f}}}\n",
               nlohmann::json(name).dump(), obj, bound, runtime);
}
