cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DGUROBI_DIR=/opt/gurobi/linux64
cmake --build build -j 8

mkdir -p bin
find build/src -perm +111 -type f -exec cp {} bin/ \;
