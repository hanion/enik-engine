cmake_minimum_required(VERSION 3.26.4)

project(engine_env)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(editor)
add_subdirectory(runtime)
add_subdirectory(engine)