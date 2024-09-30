set(IO_SOURCES "IO.cpp")
add_executable(IO ${IO_SOURCES})
target_include_directories(IO PRIVATE "../include")

set(ERR_SOURCES "Errors.cpp")
add_executable(Errors ${ERR_SOURCES})
target_include_directories(Errors PRIVATE "../include")
