cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DGUROBI_DIR=/opt/gurobi/linux64
cmake --build build -j 8

mkdir -p bin
find build/src -type f -executable -exec cp {} bin/ \;
