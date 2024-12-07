add_executable(coord_single main_coord.cpp coord.cpp instance.cpp cli.cpp mipresult.cpp bottom_left.cpp)

target_include_directories(coord_single PUBLIC ${GUROBI_INCLUDE_DIRS})
target_link_libraries(coord_single PUBLIC ${GUROBI_CPP_LIBRARY} ${GUROBI_LIBRARY} fmt::fmt -pthread)

add_executable(bottom_left main_bottom_left.cpp bottom_left.cpp instance.cpp cli.cpp heuresult.cpp)
target_link_libraries(bottom_left PUBLIC fmt::fmt -pthread)