cmake_minimum_required(VERSION 3.26.4)


project(engine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") #-Werror ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -DTRACY_ENABLE -Itracy/")

set (CMAKE_CXX_STANDARD 14)

set(source_dir "${PROJECT_SOURCE_DIR}/src/")

file(GLOB source_files 
	"${source_dir}/*.cpp" 
	"${source_dir}/core/*.cpp"
	"${source_dir}/layers/*.cpp"
	"${source_dir}/layers/imgui_layer/*.cpp"
	"${source_dir}/renderer/*.cpp"
	"${source_dir}/renderer/opengl/*.cpp"
	"${PROJECT_SOURCE_DIR}/external/stb_image/*.cpp"
)
file(GLOB include_files 
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${source_dir}/core/*.h"
	"${source_dir}/events/*.h"
	"${source_dir}/layers/*.h"
	"${source_dir}/layers/imgui_layer/*.h"
	"${source_dir}/renderer/*.h"
	"${source_dir}/renderer/opengl/*.h"
	"${PROJECT_SOURCE_DIR}/external/stb_image/*.h"
)



add_library(${PROJECT_NAME} SHARED ${source_files} ${include_files})

add_subdirectory(external/spdlog)
add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/imgui)
add_subdirectory(external/glm)


target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:EN_DEBUG> $<$<CONFIG:Debug>:TRACY_ENABLE>)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_subdirectory(external/tracy)
	target_include_directories(${PROJECT_NAME} PUBLIC external/tracy/pubic/tracy)
	target_link_libraries(${PROJECT_NAME} PUBLIC tracy)
endif()


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