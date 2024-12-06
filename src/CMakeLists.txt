add_executable(brkga2 main_heuristic.cpp heuristic.cpp instance.cpp cli.cpp heuresult.cpp bottom_left.cpp Population.cpp decoder.cpp)
target_include_directories(brkga2 PUBLIC libs/brkgaAPI)
target_link_libraries(brkga2 PUBLIC fmt::fmt -pthread OpenMP::OpenMP_CXX)
