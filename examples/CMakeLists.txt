add_executable(Errors Errors.cpp)
add_executable(IO IO.cpp)
add_executable(Defer Defer.cpp)

target_link_libraries(Errors PRIVATE mlibLogger)
target_link_libraries(IO PRIVATE mlibUtils)
