
include_directories(
    ${KOFFICECORE_INCLUDES} ${CMAKE_BINARY_DIR}/filters/ 
    )

add_subdirectory( export )
add_subdirectory( import )

configure_file(config-xsltfilter.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-xsltfilter.h )

