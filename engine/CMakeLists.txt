cmake_minimum_required(VERSION 3.26.4)


project(engine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") #-Werror ")

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

set(source_dir "${PROJECT_SOURCE_DIR}/src/")
file(GLOB source_files "${source_dir}/*.cpp")
file(GLOB include_files "${PROJECT_SOURCE_DIR}/include/*.h")

add_library(${PROJECT_NAME} SHARED ${source_files} ${include_files})

add_subdirectory(external/spdlog)
add_subdirectory(external/glfw)

target_include_directories(${PROJECT_NAME}
	PUBLIC
	$<INSTALL_INTERFACE:include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

	PRIVATE
	external/spdlog/include/spdlog
	external/glfw/include/GLFW
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
	spdlog
	glfw
	PRIVATE

)

target_precompile_headers(${PROJECT_NAME}
	PUBLIC
	include/pch.h
)