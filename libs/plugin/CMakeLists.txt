
set(koplugin_LIB_SRCS
    KoPluginLoader.cpp
)

add_library(koplugin SHARED ${koplugin_LIB_SRCS})
generate_export_header(koplugin BASE_NAME koplugin)

target_link_libraries(koplugin
    PUBLIC
        Qt6::Core
    PRIVATE
        KF6::ConfigCore
        KF6::CoreAddons
)

set_target_properties(koplugin PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS koplugin ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

