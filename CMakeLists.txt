cmake_minimum_required(VERSION 3.5)
set(projectName "mlib")

add_subdirectory(Logger)
add_subdirectory(Utils)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

