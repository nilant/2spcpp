add_executable(lp1_plus main_coord.cpp coord.cpp instance.cpp cli.cpp mipresult.cpp bottom_left.cpp)

target_include_directories(lp1_plus PUBLIC ${GUROBI_INCLUDE_DIRS})
target_link_libraries(lp1_plus PUBLIC ${GUROBI_CPP_LIBRARY} ${GUROBI_LIBRARY} fmt::fmt -pthread)

add_executable(lp2_plus main_coord2.cpp coord2.cpp instance.cpp cli.cpp mipresult.cpp bottom_left_plus.cpp)

target_include_directories(lp2_plus PUBLIC ${GUROBI_INCLUDE_DIRS})
target_link_libraries(lp2_plus PUBLIC ${GUROBI_CPP_LIBRARY} ${GUROBI_LIBRARY} fmt::fmt -pthread)