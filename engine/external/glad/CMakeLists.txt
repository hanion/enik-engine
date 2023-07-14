cmake_minimum_required(VERSION 3.26.4)

project(glad)


file(GLOB include_files "${PROJECT_SOURCE_DIR}/include/glad/*.h")
file(GLOB additional_include_files "$${PROJECT_SOURCE_DIR}/include/KHR/*.h")
list(APPEND include_files ${additional_include_files})

add_library(glad src/glad.c ${include_files})
target_include_directories(glad PUBLIC include/)