#include "heuresult.hpp"

#include <fmt/core.h>

void HeurResult::print() {
    fmt::print("[RESULT] {{\"obj\":{:.2f},\"bound\":{:.2f},\"runtime\":{:.2f}}}\n",
               obj, bound, runtime);
}
