
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_BINARY_DIR}/../
)

set (core_LIB_SRCS
)

add_library(calligrasheetscore SHARED ${calligrasheetscore_LIB_SRCS})

target_link_libraries(calligrasheetscore
    PUBLIC
        calligrasheetsengine
        komain
)

set_target_properties(calligrasheetscore PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS calligrasheetscore ${INSTALL_TARGETS_DEFAULT_ARGS})

