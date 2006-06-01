

include_directories(
    ${KOFFICECORE_INCLUDES} ${CMAKE_BINARY_DIR}/filters/ 
    )

add_subdirectory( export )
add_subdirectory( import )

########### install files ###############


