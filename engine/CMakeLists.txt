cmake_minimum_required(VERSION 3.26.4)


project(engine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") #-Werror ")

set (CMAKE_CXX_STANDARD 14)

set(source_dir "${PROJECT_SOURCE_DIR}/src/")

file(GLOB source_files 
	"${source_dir}/*.cpp" 
	"${source_dir}/layers/*.cpp"
	"${source_dir}/layers/imgui_layer/*.cpp"
	"${source_dir}/renderer/*.cpp"
	"${source_dir}/renderer/opengl/*.cpp"
	"${PROJECT_SOURCE_DIR}/external/stb_image/*.cpp"
	)
	file(GLOB include_files 
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${source_dir}/events/*.h"
	"${source_dir}/layers/*.h"
	"${source_dir}/layers/imgui_layer/*.h"
	"${source_dir}/renderer/*.h"
	"${source_dir}/renderer/opengl/*.h"
	"${PROJECT_SOURCE_DIR}/external/stb_image/*.h"
)


# these fix the undefined reference problem
# turns out, we need to add every subdirectory we create to compile them
# file(GLOB additional_files "${source_dir}/layers/*.cpp")
# list(APPEND source_files ${additional_files})
# 
# file(GLOB additional_include_files "${source_dir}/events/*.h")
# list(APPEND include_files ${additional_include_files})

add_library(${PROJECT_NAME} SHARED ${source_files} ${include_files})

add_subdirectory(external/spdlog)
add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/imgui)
add_subdirectory(external/glm)

target_include_directories(${PROJECT_NAME}
	PUBLIC
	$<INSTALL_INTERFACE:include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

	external/spdlog/include/spdlog
	external/glfw/include/GLFW
	external/glad/include/glad
	external/imgui
	external/glm/glm

	external
	src
	include

)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
	spdlog
	glfw
	glad
	imgui
	glm
	PRIVATE

)

target_precompile_headers(${PROJECT_NAME}
	PUBLIC
	include/pch.h
)