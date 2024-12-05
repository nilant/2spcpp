add_executable(brkga1 main_heuristic.cpp heuristic.cpp instance.cpp cli.cpp heuresult.cpp bottom_left.cpp Population.cpp decoder.cpp)
target_include_directories(brkga1 PUBLIC libs/brkgaAPI)
target_link_libraries(brkga1 PUBLIC fmt::fmt -pthread OpenMP::OpenMP_CXX)
