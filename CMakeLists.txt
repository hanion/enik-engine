cmake_minimum_required(VERSION 3.26.4)


project(engine_env)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") #-Werror ")

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(editor)
add_subdirectory(sandbox)
add_subdirectory(engine)