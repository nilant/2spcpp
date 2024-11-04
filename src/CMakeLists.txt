add_executable(heur100 main_heuristic.cpp heuristic.cpp instance.cpp cli.cpp heuresult.cpp shelves.cpp coord.cpp mipresult.cpp assignment.cpp bottom_left_plus.cpp)
 
target_include_directories(heur100 PUBLIC ${GUROBI_INCLUDE_DIRS})
target_link_libraries(heur100 PUBLIC ${GUROBI_CPP_LIBRARY} ${GUROBI_LIBRARY} fmt::fmt -pthread)