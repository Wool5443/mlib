set(LIB_NAME MLIB_UTILS)

add_library(${LIB_NAME} INTERFACE )
target_include_directories(${LIB_NAME} INTERFACE include)
target_link_libraries(${LIB_NAME} INTERFACE MLIB_LOGGER)
