cmake_minimum_required(VERSION 3.15)
set(projectName "mlib")
project(${projectName})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -fsanitize=address")

add_subdirectory(Logger)
add_subdirectory(Utils)

find_package(fmt)
target_link_libraries(mlibLogger INTERFACE fmt::fmt)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
