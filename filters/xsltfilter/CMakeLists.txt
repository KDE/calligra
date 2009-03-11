
include_directories(
    ${KOMAIN_INCLUDES} ${CMAKE_BINARY_DIR}/filters/ 
    )

add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

add_subdirectory( export )
add_subdirectory( import )

configure_file(config-xsltfilter.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-xsltfilter.h )

