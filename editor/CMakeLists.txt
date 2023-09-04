cmake_minimum_required(VERSION 3.26.4)


project(editor)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") #-Werror ")

set (CMAKE_CXX_STANDARD 17)

set(source_dir "${PROJECT_SOURCE_DIR}/src/")

file(GLOB source_files
	"${source_dir}/*.cpp"
	"${source_dir}/panels/*.cpp"
	"${source_dir}/dialogs/*.cpp"
	"${source_dir}/utils/*.cpp"
)

file(GLOB include_files
	"${source_dir}/*h"
	"${source_dir}/panels/*.h"
	"${source_dir}/dialogs/*.h"
	"${source_dir}/utils/*.h"
)

add_executable(${PROJECT_NAME} ${source_files} ${include_files})

target_link_libraries(${PROJECT_NAME} enik-engine)
target_include_directories(${PROJECT_NAME} PRIVATE enik-engine editor)