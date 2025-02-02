cmake_minimum_required(VERSION 3.26.4)

project(runtime)

if(MSVC)
	set(CMAKE_CXX_FLAGS "/w /FS /wd4820 /wd4996 ${CMAKE_CXX_FLAGS_INIT}") # Suppress all warnings on MSVC
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-dangling-reference")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(source_dir "${PROJECT_SOURCE_DIR}/src/")
file(GLOB source_files "${source_dir}/*.cpp")
file(GLOB include_files "${source_dir}/*h")

add_executable(${PROJECT_NAME} ${source_files} ${include_files})

if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath='$ORIGIN'")
endif()

if(NOT (MINGW OR MSVC))
	target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
endif()

target_link_libraries(${PROJECT_NAME} enik-engine)
target_include_directories(${PROJECT_NAME} PRIVATE enik-engine)