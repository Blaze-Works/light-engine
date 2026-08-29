ycmake_minimum_required(VERSION 3.14)
project(light_engine VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_SHARED_LIBS "Build shared/dynamic libraries instead of static" OFF)
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(INCLUDE_EXAMPLES "Build example applications" ON)

find_package(glfw3 CONFIG REQUIRED)
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

if(BUILD_SHARED_LIBS)
	target_compile_definitions(${PROJECT_NAME} PRIVATE LIGHT_ENGINE_EXPORT)
else()
	target_compile_definitions(${PROJECT_NAME} PUBLIC LIGHT_ENGINE_EXPORT)
endif()

if(ENABLE_ASAN AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=address -g -fno-omit-frame-pointer)
	target_link_options(${PROJECT_NAME} PUBLIC -fsanitize=address)
endif()

file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp" "src/*.c")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

add_library(${PROJECT_NAME} ${ENGINE_SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE LIGHT_ENGINE_EXPORT)

if (NOT BUILD_SHARED_LIBS)
	target_compile_definitions(${PROJECT_NAME} PUBLIC LIGHT_ENGINE_EXPORT)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	${GLFW_LIBRARIES}
	OpenGL::GL
	glm::glm
PUBLIC
	Freetype::Freetype
)

function(add_example EXAMPLE_NAME)
	set(options "")
	set(oneValueArgs "")
	set(multiValueArgs ASSETS)

	cmake_parse_arguments(PARSE_ARGV 1 ARG "${options}" "${oneValueArgs}" "${multiValueArgs}")

	add_executable(example_${EXAMPLE_NAME} "examples/${EXAMPLE_NAME}/main.cpp")
	target_link_libraries(example_${EXAMPLE_NAME} PRIVATE ${PROJECT_NAME})

	# Copy assets automatically at compile time if they are updated
	foreach(ASSET IN LISTS ARG_ASSETS)
		set(SOURCE_ASSET_PATH "${CMAKE_CURRENT_SOURCE_DIR}/examples/${EXAMPLE_NAME}/${ASSET}")
		set(TARGET_ASSET_PATH "${CMAKE_CURRENT_BINARY_DIR}/resources/${ASSET}")

		if(EXISTS "${SOURCE_ASSET_PATH}")
			add_custom_command(
				TARGET example_${EXAMPLE_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
					"${SOURCE_ASSET_PATH}"
					"${TARGET_ASSET_PATH}"
				COMMENT "Syncing asset: ${ASSET} for example_${EXAMPLE_NAME}"
			)
		else()
			message(WARNING "Asset source not found for example_${EXAMPLE_NAME}: ${ASSET}")
		endif()
	endforeach()
endfunction()


if(INCLUDE_EXAMPLES)
	# Target Definitions
	add_example(pong)
	add_example(multithreaded)
	add_example(cube)
	add_example(screen)
	add_example(texture)
	add_example(3dmodel)
	add_example(camera-control)
	add_example(animation)

	# Threading support specifically for multithreaded target
	find_package(Threads REQUIRED)
	target_link_libraries(example_multithreaded PRIVATE Threads::Threads)
endif()
