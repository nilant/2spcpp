#include <algorithm>
#include <fmt/core.h> 
#include <limits>
#include <random>

#include "bottom_left.hpp"
#include "cli.hpp"
#include "coord.hpp"
#include "gurobi_c.h"
#include "instance.hpp"
#include "mdarray.hpp"

bool fit(Config const& item, mdarray<int, 2> const& coord, int p, int q) {
    bool flag = true;
    for (int i = p; i < p + item.w; i++) {
        for (int j = q; j < q + item.h; j++) {
            flag = flag && coord(i, j);
        }
    }

    return flag;
}

void fill_space(mdarray<int, 2>& coord, int p, int q, Config& item) {
    for (int i = p; i < p + item.w; ++i) {
        for (int j = q; j < q + item.h; ++j) {
            coord(i, j) = false;
        }
    }
    item.x = p;
    item.y = q;
}

int bottom_left_impl(std::vector<Config>::iterator begin, std::vector<Config>::iterator end, int w, int ub) {

    mdarray<int, 2> coord{w, ub};
 
    for (int p = 0; p < w; ++p) {
        for (int q = 0; q < ub; ++q) {
            coord(p, q) = true;
        }
    }

    int obj = 0;
    for (auto it = begin; it != end; ++it) {
        auto& item = *it;
        bool flag = false;
        for (int q = 0; q <= ub - item.h; ++q) {
            for (int p = 0; p <= w - item.w; ++p) {
                if (fit(item, coord, p, q)) {
                    flag = true;
                    obj = std::max(q + item.h, obj);
                    fill_space(coord, p, q, item);
                    goto next_item;
                }
            }
        }
        if (!flag) {
            #ifndef NDEBUG
                fmt::print("item {} does not fit\n", item.id);
            #endif
            return std::numeric_limits<int>::max();
        }
        next_item:;
    }

    return obj;
}

HeurResult bottom_left(Instance const& inst) {

    auto t0 = std::chrono::high_resolution_clock::now();

    HeurResult res{"bottom_left"};

    std::vector<Config> items{inst.selected_items};

    int ub = 2*inst.ub;
    int lb = 0;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::bernoulli_distribution d(0.1);

    auto mid = std::partition(items.begin(), items.end(), [&](auto const& a) {
                                                            auto val = !d(gen); 
                                                            return val;
                                                        });
    std::sort(items.begin(), mid, 
            [](auto const& a, auto const& b) {
                if ((a.h > b.h) || (a.h == b.h && a.w < b.w)) {
                    return true;
                }
                return false;
            }
    );

    #ifndef NDEBUG
        fmt::print("keeping out items ");
        for (auto it = mid; it != items.end(); ++it) {
            fmt::print("{}, ", it->id);
        }
        fmt::print("\n");
    #endif

    int obj = std::numeric_limits<int>::max();
    for (auto it = mid; it != items.end(); ++it) {
        Config best_config;
        int task_id = it->task_id;
        for (auto& config : inst.tasks[task_id].configs) {
            *it = config;
            int new_obj = bottom_left_impl(items.begin(), std::next(it), inst.w, ub);
            if (new_obj < obj) {
                obj = new_obj;
                best_config = config;
            }
        }
        *it = best_config;
    }

    obj = bottom_left_impl(items.begin(), items.end(), inst.w, ub);

    #ifndef NDEBUG
    if (!check_feas(items)) {
        std::exit(1);
    }
    #endif

    auto t1 = std::chrono::high_resolution_clock::now();
    res.obj = obj;
    res.bound = lb;
	res.runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0; 

    return res;
}