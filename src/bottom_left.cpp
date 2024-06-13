#include <algorithm>
#include <cmath>
#include <fmt/core.h> 
#include <limits>

#include <fmt/core.h>

#include "bottom_left.hpp"
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
        for (int q = 0; q <= ub - item.h; ++q) {
            for (int p = 0; p <= w - item.w; ++p) {
                if (fit(item, coord, p, q)) {
                    obj = std::max(q + item.h, obj);
                    fill_space(coord, p, q, item);
                    goto next_item;
                }
            }
        }
        next_item:;
    }

    #ifndef NDEBUG
        print_solution(begin, end, obj);
    #endif

    return obj;
}

HeurResult bottom_left(Instance const& inst) {

    auto t0 = std::chrono::high_resolution_clock::now();

    HeurResult res{"bottom_left"};

    std::vector<Config> items{inst.selected_items};

    int ub = 2*inst.ub;
    int lb = 0;

    std::sort(items.begin(), items.end(), 
                [](auto const& a, auto const& b) {
                    if ((a.h > b.h) || (a.h == b.h && a.w < b.w)) {
                        return true;
                    }
                    return false;
                }
        );

    auto mid = items.begin() + std::lrint(std::ceil(items.size() * 0.9));
    for (auto it = mid; it != items.end(); ++it) {

        int task_id = items.back().task_id;
        Config best_config;
        int best_obj = std::numeric_limits<int>::max(); 
        for (auto& config : inst.tasks[task_id].configs) {
            *it = config;
            int new_obj = bottom_left_impl(items.begin(), it+1, inst.w, ub);
            if (new_obj < best_obj) {
                best_obj = new_obj;
                best_config = config;
            }
        }
        *it = best_config;
    }

    int final_obj = bottom_left_impl(items.begin(), items.end(), inst.w, ub);

    #ifndef NDEBUG
    if (!check_feas(items)) {
        std::exit(1);
    }
    #endif

    auto t1 = std::chrono::high_resolution_clock::now();
    res.obj = final_obj;
    res.bound = lb;
	res.runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0; 

    return res;
}