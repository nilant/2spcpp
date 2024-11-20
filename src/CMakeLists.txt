add_executable(math2 main_heuristic.cpp heuristic.cpp instance.cpp cli.cpp heuresult.cpp shelves.cpp coord.cpp mipresult.cpp assignment.cpp bottom_left_plus.cpp)
target_include_directories(math2 PUBLIC ${GUROBI_INCLUDE_DIRS})
target_link_libraries(math2 PUBLIC ${GUROBI_CPP_LIBRARY} ${GUROBI_LIBRARY} fmt::fmt -pthread)
