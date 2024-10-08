cmake_minimum_required(VERSION 3.26.4)

project(enik-engine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-dangling-reference -Wl,-E")

# set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(source_dir "${PROJECT_SOURCE_DIR}/src/")

file(
  GLOB
  source_files
  "${source_dir}/*.cpp"
  "${source_dir}/core/*.cpp"
  "${source_dir}/layers/*.cpp"
  "${source_dir}/layers/imgui_layer/*.cpp"
  "${source_dir}/renderer/*.cpp"
  "${source_dir}/renderer/opengl/*.cpp"
  "${PROJECT_SOURCE_DIR}/external/stb_image/*.cpp"
  "${PROJECT_SOURCE_DIR}/external/entt/*.hpp"
  "${PROJECT_SOURCE_DIR}/external/filewatch/*.hpp"
  "${source_dir}/scene/*.cpp"
  "${source_dir}/project/*.cpp"
  "${source_dir}/script_system/*.cpp"
  "${source_dir}/physics/*.cpp"
  "${source_dir}/audio/*.cpp")
file(
  GLOB
  include_files
  "${PROJECT_SOURCE_DIR}/include/*.h"
  "${source_dir}/core/*.h"
  "${source_dir}/events/*.h"
  "${source_dir}/layers/*.h"
  "${source_dir}/layers/imgui_layer/*.h"
  "${source_dir}/renderer/*.h"
  "${source_dir}/renderer/opengl/*.h"
  "${PROJECT_SOURCE_DIR}/external/stb_image/*.h"
  "${PROJECT_SOURCE_DIR}/external/entt/*.hpp"
  "${PROJECT_SOURCE_DIR}/external/filewatch/*.hpp"
  "${source_dir}/scene/*.h"
  "${source_dir}/project/*.h"
  "${source_dir}/script_system/*.h"
  "${source_dir}/physics/*.h"
  "${source_dir}/audio/*.h")


if(UNIX)
  add_library(${PROJECT_NAME} SHARED ${source_files} ${include_files})
endif()
if(WIN32)
  add_library(${PROJECT_NAME} STATIC ${source_files} ${include_files})
endif()

set(BUILD_SHARED_LIBS OFF)
set(YAML_BUILD_SHARED_LIBS OFF)

add_subdirectory(external/spdlog)
add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/imgui)
add_subdirectory(external/glm)
add_subdirectory(external/yaml-cpp)

# automatically set platform globally
if(UNIX AND NOT APPLE)
  set(EN_PLATFORM_LINUX 1)
endif()

if(WIN32)
  set(EN_PLATFORM_WINDOWS 1)
endif()

target_compile_definitions(${PROJECT_NAME}
	PUBLIC
	$<$<BOOL:${EN_PLATFORM_LINUX}>:EN_PLATFORM_LINUX>
	$<$<BOOL:${EN_PLATFORM_WINDOWS}>:EN_PLATFORM_WINDOWS>
	ENIK_ENGINE_BUILD
	$<$<CONFIG:Debug>:EN_DEBUG>
)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	cmake_policy(SET CMP0077 NEW)
	set(TRACY_STATIC ON)
	add_subdirectory(external/tracy)
	target_include_directories(${PROJECT_NAME} PUBLIC external/tracy/public/tracy)
	target_link_libraries(${PROJECT_NAME} PUBLIC TracyClient)
	set_property(TARGET TracyClient PROPERTY POSITION_INDEPENDENT_CODE ON)
	target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:TRACY_ENABLE>)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTRACY_ENABLE -Itracy/")
endif()

target_include_directories(
  ${PROJECT_NAME}
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         external/spdlog/include/spdlog
         external/glfw/include/GLFW
         external/glad/include/glad
         external/imgui
         external/glm/glm
         external/yaml-cpp
         external
         src
         include)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC spdlog glfw glad imgui glm yaml-cpp
  PRIVATE)

target_precompile_headers(${PROJECT_NAME} PUBLIC include/pch.h)