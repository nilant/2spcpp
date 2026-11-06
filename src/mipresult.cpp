#include <fmt/core.h>

#include "mipresult.hpp"
#include "../libs/json.hpp"
#include "gurobi_c++.h"
#include "gurobi_c.h"

void MIPResult::print() {
    nlohmann::ordered_json j;
    j["obj"] = obj;
    j["bound"] = bound;
    j["gap"] = gap;
    j["runtime"] = runtime;
    fmt::print("{}\n", j.dump());
}

// Set seed/time/memory/thread limits from `args`, optimize and fill the
// result. If no solution is found the fields are set to -1. In debug builds,
// infeasible models are dumped to <name>.lp / <name>.ilp (IIS).
MIPResult solve(std::string const& name, GRBModel& model, Args const& args) {

    model.set(GRB_IntParam_Seed, args.seed);
    model.set(GRB_DoubleParam_TimeLimit, args.timelimit);
    model.set(GRB_DoubleParam_SoftMemLimit, args.memlimit);
    model.set(GRB_IntParam_Threads, args.threads);

    MIPResult result{name};
    try {
        model.optimize();
        if (model.get(GRB_IntAttr_SolCount) > 0) {
            result.obj = model.get(GRB_DoubleAttr_ObjVal);
            result.bound = model.get(GRB_DoubleAttr_ObjBound);
            result.gap = model.get(GRB_DoubleAttr_MIPGap);
            result.runtime = model.get(GRB_DoubleAttr_Runtime);
        } else {
            result.obj = -1;
            result.bound = -1;
            result.gap = -1;
            result.runtime = -1;
        }

        if (model.get(GRB_IntAttr_Status) == GRB_INFEASIBLE) {
            #ifndef NDEBUG
                model.computeIIS();
                model.write(fmt::format("{}.lp", name));
                model.write(fmt::format("{}.ilp", name));
            #endif
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