set(ERR_SOURCES "IO.cpp")
add_executable(IO ${ERR_SOURCES})
target_include_directories(IO PRIVATE "include" "../Logger/include")
