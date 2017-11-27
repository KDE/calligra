
set(koplugin_LIB_SRCS
    KoPluginLoader.cpp
)

add_library(planplugin SHARED ${koplugin_LIB_SRCS})
generate_export_header(planplugin BASE_NAME koplugin)

target_link_libraries(planplugin
    PUBLIC
        Qt5::Core
    PRIVATE
        KF5::ConfigCore
        KF5::CoreAddons
)

set_target_properties(planplugin PROPERTIES
    VERSION ${GENERIC_PLAN_LIB_VERSION} SOVERSION ${GENERIC_PLAN_LIB_SOVERSION}
)
install(TARGETS planplugin ${INSTALL_TARGETS_DEFAULT_ARGS} )

