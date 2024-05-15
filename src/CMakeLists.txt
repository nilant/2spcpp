add_executable(matheuristic main_heuristic.cpp heuristic.cpp instance.cpp cli.cpp heuresult.cpp shelves.cpp coord.cpp mipresult.cpp assignment.cpp bottom_left.cpp)

target_include_directories(matheuristic PUBLIC ${GUROBI_INCLUDE_DIRS})
target_link_libraries(matheuristic PUBLIC ${GUROBI_CPP_LIBRARY} ${GUROBI_LIBRARY} fmt::fmt -pthread)
