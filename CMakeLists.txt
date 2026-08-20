cmake_minimum_required(VERSION 3.14)
project(LIGHT_ENGINE VERSION 1.0.0 LANGUAGES C CXX)

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

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.c" "examples/multithreaded/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${X11_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
	${GLFW_LIBRARIES} 
	OpenGL::GL 
	Freetype::Freetype
	glm::glm
	m 
	X11 
	Xrandr 
	Xi
)
