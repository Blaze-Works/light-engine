mkdir -p build
cd build

# Configure and compile
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON -DINCLUDE_EXAMPLES=ON -G "Ninja" ..
ninja

export DISPLAY=:0
export LIBGL_ALWAYS_SOFTWARE=1
export MESA_GL_VERSION_OVERRIDE=3.0
export MESA_DEBUG=1

# Run the program
lldb ./example_animation
