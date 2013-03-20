include_directories(${KOMVC_INCLUDES})

set(komvc_LIB_SRCS
)

kde4_add_library(komvc SHARED ${komvc_LIB_SRCS})

target_link_libraries(komvc koplugin)

set_target_properties(komvc PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)

install(TARGETS komvc  ${INSTALL_TARGETS_DEFAULT_ARGS})

install( FILES
    komvc_export.h

    DESTINATION
    ${INCLUDE_INSTALL_DIR}
COMPONENT Devel)
