add_executable(coord main_coord.cpp coord.cpp instance.cpp cli.cpp mipresult.cpp)

target_include_directories(coord PUBLIC ${GUROBI_INCLUDE_DIRS})
target_link_libraries(coord PUBLIC ${GUROBI_CPP_LIBRARY} ${GUROBI_LIBRARY} fmt::fmt -pthread)

add_executable(coord2 main_coord2.cpp coord2.cpp instance.cpp cli.cpp mipresult.cpp)

target_include_directories(coord2 PUBLIC ${GUROBI_INCLUDE_DIRS})
target_link_libraries(coord2 PUBLIC ${GUROBI_CPP_LIBRARY} ${GUROBI_LIBRARY} fmt::fmt -pthread)

add_executable(shelves main_shelves.cpp shelves.cpp instance.cpp cli.cpp mipresult.cpp)

target_include_directories(shelves PUBLIC ${GUROBI_INCLUDE_DIRS})
target_link_libraries(shelves PUBLIC ${GUROBI_CPP_LIBRARY} ${GUROBI_LIBRARY} fmt::fmt -pthread)

add_executable(bottom_left main_bottom_left.cpp bottom_left.cpp instance.cpp cli.cpp mipresult.cpp)
target_include_directories(bottom_left PUBLIC ${GUROBI_INCLUDE_DIRS})
target_link_libraries(bottom_left PUBLIC ${GUROBI_CPP_LIBRARY} ${GUROBI_LIBRARY} fmt::fmt -pthread)