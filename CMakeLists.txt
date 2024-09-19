cmake_minimum_required(VERSION 3.5)
set(projectName "test")
project(${projectName} CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_CXX_FLAGS_DEBUG
"-Wno-maybe-uninitialized -Wno-switch -Wno-conversion -Wno-parentheses              \
-Wno-unused-variable -Wno-pointer-arith -g -D _DEBUG -ggdb3 -O0 -Wall -Wextra       \
-Weffc++ -Waggressive-loop-optimizations -Wcast-align                               \
-Wchar-subscripts -Wconditionally-supported -Wctor-dtor-privacy -Wempty-body        \
-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2                \
-Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked         \
-Winit-self -Wredundant-decls -Wsign-promo -Wstrict-null-sentinel                   \
-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods            \
-Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef       \
-Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix    \
-Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs     \
-Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector                \
-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Werror=vla        \
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,\
float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,\
object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,\
undefined,unreachable,vla-bound,vptr")

set(CMAKE_CXX_FLAGS_RELEASE
"-Wall -Wno-maybe-uninitialized -Wno-switch -Wno-unused                             \
-Wno-conversion -Wno-unused-variable -Wno-pointer-arith -Wextra -march=native       \
-mavx512f -O3 -DNDEBUG")

FILE(GLOB SOURCES_CPP src/*.cpp example/main.cpp
                      tests/Tests.cpp)

file(MAKE_DIRECTORY "dump/list")
file(MAKE_DIRECTORY "dump/list/txt")
file(MAKE_DIRECTORY "dump/list/dot")
file(MAKE_DIRECTORY "dump/list/img")

file(MAKE_DIRECTORY "dump/tree")
file(MAKE_DIRECTORY "dump/tree/dot")
file(MAKE_DIRECTORY "dump/tree/img")

set(MLIB_SOURCES
    ${SOURCES_CPP}
)

set(SOURCES
    example/main.cpp
    tests/Tests.cpp
)

add_library(mlib STATIC ${MLIB_SOURCES})
target_include_directories(mlib PUBLIC headers tests)

add_executable(${projectName} ${SOURCES})

target_link_libraries(${projectName} mlib)
