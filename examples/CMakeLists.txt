add_executable(Errors Errors.cpp)
add_executable(IO IO.cpp)

target_link_libraries(Errors PRIVATE MLIB_LOGGER)
target_link_libraries(IO PRIVATE MLIB_UTILS)
