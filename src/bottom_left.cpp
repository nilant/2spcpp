#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fmt/core.h> 

#include <fmt/core.h>

#include "bottom_left.hpp"
#include "instance.hpp"
#include "mdarray.hpp"

bool fit(Config const& item, mdarray<int, 2> const& coord, int p, int q) {
    bool flag = true;
    for (int i = p; i < p + item.w; i++) {
        for (int j = q; j < q + item.h; j++) {
            flag = flag && (coord(i, j) == -1);
        }
    }

    return flag;
}

void fill_space(mdarray<int, 2>& coord, std::vector<int>& tops, int p, int q, Config& item) {
    for (int i = p; i < p + item.w; ++i) {
        for (int j = q; j < q + item.h; ++j) {
            coord(i, j) = item.id;
        }
    }

    item.x = p;
    item.y = q;

    tops[item.id] = item.y + item.h;
}

int find_adjacent_item_id(mdarray<int, 2>& coord, int p, int q, int w, int h, int strip_w, bool left) {

    int item_id = -1;
    // if (left) {
    //     for (int j = q; j < q + h; ++j) {
    //         if (coord(p-1, j) != -1) {
    //             item_id = coord(p-1, j);
    //         }
    //     }
    // } else {
    //     for (int j = q; j < q + h; ++j) {
    //         if (coord(p+w, j) != -1) {
    //             item_id = coord(p+w, j);
    //         }
    //     }
    // } 

    if (left && p != 0) {
        item_id = coord(p-1, q);
    } else if (p < strip_w - w) {
        item_id = coord(p+w, q);
    }
    return item_id;
}

int get_top_from_id(std::vector<int>& tops, int id, int ub) {
    if (id == -1) {
        return ub;
    }
    return tops[id];
}

int bottom_left_impl(std::vector<Config>::iterator begin, std::vector<Config>::iterator end, int w, int ub) {

    mdarray<int, 2> coord{w, ub};
 
    for (int p = 0; p < w; ++p) {
        for (int q = 0; q < ub; ++q) {
            coord(p, q) = -1;
        }
    }

    std::vector<int> tops(end-begin);

    int obj = 0;
    for (auto it = begin; it != end; ++it) {
        auto& item = *it;
        for (int q = 0; q <= ub - item.h; ++q) {
            int first_p = w;
            int last_p = -1;
            for (int p = 0; p <= w - item.w; ++p) {
                if (fit(item, coord, p, q)) {
                    first_p = std::min(first_p, p);
                    last_p = std::max(last_p, p);
                }
            }

            int left_item_id = find_adjacent_item_id(coord, first_p, q, item.w, item.h, w, true);
            int left_item_top = get_top_from_id(tops, left_item_id, ub);

            int right_item_id = find_adjacent_item_id(coord, first_p, q, item.w, item.h, w, false);
            int right_item_top = get_top_from_id(tops, right_item_id, ub);

            int selected_p = -1;
            if (first_p == w && last_p == -1) {
                continue;
            } else if (first_p == -1) {
                selected_p = last_p;
            } else if (last_p == w) {
                selected_p = first_p;
            } else if (q + item.h <= left_item_top && q + item.h > right_item_top) {
                selected_p = first_p;
            } else if (q + item.h <= right_item_top && q + item.h > left_item_top) {
                selected_p = last_p;
            } else if (std::abs((q + item.h) - left_item_top) <= std::abs(( + item.h) - right_item_top)) {
                selected_p = first_p;
            } else {
                selected_p = last_p;
            }

            assert(selected_p != -1); 
            obj = std::max(q + item.h, obj);
            fill_space(coord, tops, selected_p, q, item);
            goto next_item;
        }
        next_item:;
    }

    return obj;
}

HeurResult bottom_left(Instance& inst) {

    auto t0 = std::chrono::high_resolution_clock::now();

    HeurResult res{"bottom_left"};

    int ub = 2*inst.ub;
    int lb = 0;

    auto& items = inst.selected_items;

    std::sort(items.begin(), items.end(), 
                [](auto const& a, auto const& b) {
                    if ((a.h > b.h) || (a.h == b.h && a.w < b.w)) {
                        return true;
                    }
                    return false;
                }
        );

    auto t1 = std::chrono::high_resolution_clock::now();
    res.obj = bottom_left_impl(items.begin(), items.end(), inst.w, ub);

    #ifndef NDEBUG
    if (!check_feas(items)) {
        std::exit(1);
    }
    #endif
    res.bound = lb;
	res.runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0; 

    return res;
}