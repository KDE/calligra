
project(kivio)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kivio.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kivio.h)

#add_subdirectory( kiviopart )
#add_subdirectory( plugins )
#add_subdirectory( templates )

add_subdirectory(src)

