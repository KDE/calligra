
include_directories(
    ${CMAKE_BINARY_DIR}/kplato
    ${kplato_SOURCE_DIR}
    ${kplato_SOURCE_DIR}/libs/kernel
    ${KOMAIN_INCLUDES} ${CMAKE_BINARY_DIR}/filters/ 
)

if(KdepimLibs_FOUND)
    macro_optional_add_subdirectory( icalendar )
endif(KdepimLibs_FOUND)

