#include "heuresult.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <fmt/core.h>

#include "../libs/json.hpp"

void HeurResult::write(fs::path const& file_path) {
        std::ifstream file(file_path);
        auto j = nlohmann::ordered_json::parse(file);

        std::ofstream file_ss{file_path};
        auto& out = file_path.empty() ? std::cout : file_ss;

        out << std::setprecision(2)  << std::setw(4) << std::fixed;

        nlohmann::ordered_json jsol;
        jsol["obj"] = obj;
        jsol["runtime"] = runtime;
        jsol["status"] = status;
        j[name] = jsol;

        out << j << std::endl;
}

void HeurResult::print() {
    fmt::print("obj={}, bound={}, runtime={}\n", obj, bound, runtime);
}