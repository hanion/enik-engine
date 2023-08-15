cmake_minimum_required(VERSION 3.26.4)


project(editor)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") #-Werror ")

set (CMAKE_CXX_STANDARD 14)

set(source_dir "${PROJECT_SOURCE_DIR}/src/")
file(GLOB source_files "${source_dir}/*.cpp")
file(GLOB include_files "${source_dir}/*h")

add_executable(${PROJECT_NAME} ${source_files} ${include_files})

target_link_libraries(${PROJECT_NAME} engine)
target_include_directories(${PROJECT_NAME} PRIVATE engine)