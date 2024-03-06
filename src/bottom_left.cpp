#include <fmt/core.h> 

#include "bottom_left.hpp"
#include "instance.hpp"
#include "mdarray.hpp"
#include "mipresult.hpp"

MIPResult bottom_left(Instance const& inst) {
    
    auto t0 = std::chrono::high_resolution_clock::now();

    MIPResult res{"bottom_left"};

    mdarray<int, 2> coord{inst.ub+1, inst.w+1};

    for (int q = 0; q <= inst.ub; ++q) {
        for (int p = 0; p <= inst.w; ++p) {
            coord(q, p) = true;
        }
    }

    std::vector<Config> items;
    items.reserve(inst.ntasks * inst.rmax);
    for (auto const& task : inst.tasks) {
        for (int r = 0; r < task.repeat; ++r) {
            items.push_back(task.configs[0]);
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

    int area = 0;
    for (auto const& item : items) {
        area += item.w * item.h;
    }
    area /= 16.0;
    fmt::print("lb={}\n", area);

    int obj = 0;
    for (auto const& item : items) {
        for (int q = 0; q <= inst.ub - item.h; ++q) {
            for (int p = 0; p <= inst.w - item.w; ++p) {
                if (coord(q, p) && coord(q + item.h, p + item.w)) {
                    obj = std::max(q + item.h, obj);
                    for (int i = q; i < q + item.h; ++i) {
                        for (int j = p; j < p + item.w; ++j) {
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
	res.runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0; 
    res.obj = obj;
    res.bound = obj;

    return res;
}