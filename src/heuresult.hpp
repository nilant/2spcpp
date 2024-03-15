#pragma once
#include <filesystem>

namespace fs = std::filesystem;

struct HeurResult {
    std::string name;
    double obj;
    double runtime;

    explicit HeurResult(std::string const& alg_name) : name{alg_name} {}
    void write(fs::path const& file_path);
    void print();
};
