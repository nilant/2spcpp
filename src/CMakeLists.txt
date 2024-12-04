add_executable(brkga main_heuristic.cpp heuristic.cpp instance.cpp cli.cpp heuresult.cpp bottom_left.cpp Population.cpp decoder.cpp)
target_include_directories(brkga PUBLIC libs/brkgaAPI)
target_link_libraries(brkga PUBLIC fmt::fmt -pthread OpenMP::OpenMP_CXX)
