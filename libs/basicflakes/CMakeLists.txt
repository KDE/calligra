add_subdirectory(plugin)

include_directories(
    ${CMAKE_SOURCE_DIR}/libs/widgets
    ${PIGMENT_INCLUDES}
    ${FLAKE_INCLUDES}
)

set(basicflakes_LIB_SRCS
    tools/KoCreatePathTool.cpp
    tools/KoPencilTool.cpp
)

ki18n_wrap_ui( basicflakes_LIB_SRCS
)


add_library(basicflakes SHARED ${basicflakes_LIB_SRCS})
generate_export_header(basicflakes)

target_link_libraries(basicflakes kowidgets flake pigmentcms)

set_target_properties(basicflakes PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)

install(TARGETS basicflakes  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if (SHOULD_BUILD_DEVEL_HEADERS)

install( FILES
    tools/KoCreatePathTool.h
    tools/KoPencilTool.h
    ${CMAKE_CURRENT_BINARY_DIR}/basicflakes_export.h

    DESTINATION
    ${KDE_INSTALL_INCLUDEDIR}/calligra
COMPONENT Devel)

endif()
