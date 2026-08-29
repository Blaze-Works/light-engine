mkdir -p build
cd build

# Configure and compile
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_ASAN=ON -DINCLUDE_EXAMPLES=ON -G "Ninja" ..
ninja
