
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

target_include_directories(koplugin PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/calligra>
)

set_target_properties(koplugin PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS koplugin EXPORT CalligraQt6Targets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

