cmake_minimum_required(VERSION 3.26.4)

project(runtime)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_CXX_STANDARD 17)

set(source_dir "${PROJECT_SOURCE_DIR}/src/")
file(GLOB source_files "${source_dir}/*.cpp")
file(GLOB include_files "${source_dir}/*h")

add_executable(${PROJECT_NAME} ${source_files} ${include_files})

target_link_libraries(${PROJECT_NAME} enik-engine)
target_include_directories(${PROJECT_NAME} PRIVATE enik-engine)