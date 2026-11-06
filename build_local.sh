# Build with a local Gurobi install (expects it in /opt/gurobi/linux64) and
# copy every executable to bin/.
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DGUROBI_DIR=/opt/gurobi/linux64
cmake --build build -j 8

mkdir -p bin
find build/src -type f -executable -exec cp {} bin/ \;
