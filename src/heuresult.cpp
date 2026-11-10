#include "heuresult.hpp"

#include <filesystem>
#include <fmt/core.h>

#include "../libs/json.hpp"

void HeurResult::print() {
    nlohmann::ordered_json j;
    j["obj"] = obj;
    j["bound"] = bound;
    j["runtime"] = runtime;
    fmt::print("[RESULT] {}\n", j.dump());
}
