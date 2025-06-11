cmake_minimum_required(VERSION 3.15)
set(projectName "mlib")
project(${projectName})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_CXX_FLAGS_DEBUG
"-march=native -g -D_DEBUG -O0 -Wall -Wextra -Waggressive-loop-optimizations \
-Wcast-align -Wchar-subscripts -Wempty-body \
-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
-Wopenmp-simd -Wpacked -Winit-self \
-Wstrict-overflow=2 -Wsuggest-attribute=noreturn \
-Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wsync-nand \
-Wundef -Wunreachable-code -Wunused -Wvariadic-macros \
-Wno-missing-field-initializers -Wno-narrowing -Wstack-protector -fcheck-new \
-fstack-protector -fno-omit-frame-pointer -pie -fPIE -Werror \
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr"
)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -fPIC -DNDEBUG -march=native")

add_subdirectory(Logger)
add_subdirectory(Utils)

find_package(fmt)
target_link_libraries(mlibLogger INTERFACE fmt::fmt)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
