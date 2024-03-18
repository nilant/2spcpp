#include <cmath>
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

HeurResult bottom_left(Instance const& inst) {

    auto t0 = std::chrono::high_resolution_clock::now();

    HeurResult res{"bottom_left"};

    int ub = 0;
    std::vector<Config> items;
    items.reserve(inst.ntasks * inst.rmax);
    for (auto const& task : inst.tasks) {
        for (int r = 0; r < task.repeat; ++r) {
            items.push_back(task.configs[0]);
            ub += task.configs[0].h;
        }
    }

    int w = inst.w;
    mdarray<int, 2> coord{w, ub};

    for (int p = 0; p < w; ++p) {
        for (int q = 0; q < ub; ++q) {
            coord(p, q) = true;
        }
    }

    std::sort(items.begin(), items.end(), 
             [](auto const& a, auto const& b) { 
                if ((a.h > b.h) || (a.h == b.h && a.w < b.w)) {
                    return true;
                }
                return false;
            }
    );

    int obj = 0;
    for (auto const& item : items) {
        for (int q = 0; q <= ub - item.h; ++q) {
            for (int p = 0; p <= w - item.w; ++p) {
                if (fit(item, coord, p, q)) {
                    obj = std::max(q + item.h, obj);
                    for (int i = p; i < p + item.w; ++i) {
                        for (int j = q; j < q + item.h; ++j) {
                            coord(i, j) = false;
                        }
                    }
                    goto next_item;
                }
            }
        }
        next_item:;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    res.obj = obj;
	res.runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0; 

    return res;
}
