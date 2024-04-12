#include <iostream>
#include <fstream>
#include <fmt/core.h>

#include "mipresult.hpp"
#include "../libs/json.hpp"
#include "gurobi_c++.h"
#include "gurobi_c.h"

void MIPResult::write(fs::path const& file_path) {

        std::ifstream file(file_path);
        auto j = nlohmann::ordered_json::parse(file);

        std::ofstream file_ss{file_path};
        auto& out = file_path.empty() ? std::cout : file_ss;

        out << std::setprecision(2)  << std::setw(4) << std::fixed;

        nlohmann::ordered_json jsol;
        jsol["obj"] = obj;
        jsol["bound"] = bound;
        jsol["runtime"] = runtime;
        jsol["gap"] = gap;
        j[name] = jsol;

        out << j << std::endl;
}

void MIPResult::print() {
    fmt::print("obj={}, bound={}, runtime={}, buildtime={}\n", obj, bound, runtime, buildtime);
}

MIPResult solve(std::string const& name, GRBModel& model, Args const& args) {

    model.set(GRB_IntParam_Seed, args.seed);
    model.set(GRB_DoubleParam_TimeLimit, args.timelimit);
    model.set(GRB_DoubleParam_SoftMemLimit, args.memlimit);
    model.set(GRB_IntParam_Threads, args.threads);

    MIPResult result{name};
    try {
        model.optimize();
        if (model.get(GRB_IntAttr_SolCount) > 0) {
            result.obj = std::lround(model.get(GRB_DoubleAttr_ObjVal));
            result.bound = std::lround(model.get(GRB_DoubleAttr_ObjBound));
            result.gap = std::abs(result.obj - result.bound) / std::abs(result.obj);
            result.runtime = model.get(GRB_DoubleAttr_Runtime);
        } else {
            result.obj = -1;
            result.bound = -1;
            result.gap = -1;
            result.runtime = -1;
        }

        if (model.get(GRB_IntAttr_Status) == GRB_INFEASIBLE) {
            model.computeIIS();
            model.write(fmt::format("{}.lp", name));
            model.write(fmt::format("{}.ilp", name));
        }

    } catch(GRBException& e) {
        fmt::print("Error code = {}\n", e.getErrorCode());
        fmt::print("Error message = {}\n", e.getMessage()); 
        result.obj = -1;
        result.bound = -1;
        result.gap = -1;
        result.runtime = -1;
    } catch(...) {
        fmt::print("Exception during optimization");
        std::exit(1);
    }

    return result;
}