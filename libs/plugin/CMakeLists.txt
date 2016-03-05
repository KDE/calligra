
set(koplugin_LIB_SRCS
    KoPluginLoader.cpp
)

# help plugin trader find installed plugins when run from uninstalled tests
if (BUILD_TESTING)
    if(IS_ABSOLUTE ${PLUGIN_INSTALL_DIR})
        set(CALLIGRA_PLUGIN_INSTALL_BASE_DIR "${PLUGIN_INSTALL_DIR}")
    else()
        set(CALLIGRA_PLUGIN_INSTALL_BASE_DIR "${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_DIR}")
    endif()
    add_definitions(-DCALLIGRA_PLUGIN_INSTALL_BASE_DIR="${CALLIGRA_PLUGIN_INSTALL_BASE_DIR}")
endif()

add_library(koplugin SHARED ${koplugin_LIB_SRCS})
generate_export_header(koplugin BASE_NAME koplugin)

target_link_libraries(koplugin
    PUBLIC
        Qt5::Core
    PRIVATE
        KF5::ConfigCore
        KF5::CoreAddons
)

set_target_properties(koplugin PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS koplugin ${INSTALL_TARGETS_DEFAULT_ARGS} )

