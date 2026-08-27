cmake_minimum_required(VERSION 3.14)
project(light_engine VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(FreeType QUIET)

if (NOT Freetype_FOUND)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(FREETYPE REQUIRED freetype2)

	add_library(Freetype::Freetype INTERFACE IMPORTED)
	target_include_directories(Freetype::Freetype INTERFACE ${FREETYPE_INCLUDE_DIRS})
	target_link_libraries(Freetype::Freetype INTERFACE ${FREETYPE_LIBRARIES})
endif()

if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address -g -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp" "src/*.c")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

add_library(${PROJECT_NAME} STATIC ${ENGINE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${X11_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	${GLFW_LIBRARIES}
	OpenGL::GL
	glm::glm
	X11
	m
	Xrandr
	Xi
PUBLIC
	Freetype::Freetype
)

# Helper macro to register standard examples
macro(add_example EXAMPLE_NAME ASSET_FILES)
        add_executable(example_${EXAMPLE_NAME} examples/${EXAMPLE_NAME}/main.cpp)
        target_link_libraries(example_${EXAMPLE_NAME} PRIVATE ${PROJECT_NAME})

        foreach(ASSET ${ASSET_FILES})
		if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples/${EXAMPLE_NAME}/${ASSET})
                        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples/${EXAMPLE_NAME}/${ASSET} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
                endif()
        endforeach()
endmacro()

if(INCLUDE_EXAMPLES)
	# Target Definitions
	add_example(pong "")
	add_example(multithreaded "")
	add_example(cube "")
	add_example(screen "")
	add_example(texture "")
	add_example(3dmodel "")
	add_example(camera-control "")
	add_example(animation "")

	# Threading support specifically for multithreaded target
	find_package(Threads REQUIRED)
	target_link_libraries(example_multithreaded PRIVATE Threads::Threads)
endif()
