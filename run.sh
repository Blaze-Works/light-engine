cd build

export DISPLAY=:0
export LIBGL_ALWAYS_SOFTWARE=1
export GALLIUM_DRIVER=virtype
export MESA_GL_VERSION_OVERRIDE=3.3COMPAT
export MESA_GLSL_VERSION_OVERRIDE=330
export MESA_DEBUG=1

# Run the program
GALLIUM_DRIVER=virpipe ./example_3dmodel
