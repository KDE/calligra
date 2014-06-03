
project(flow)
include_directories(
    ${KOMAIN_INCLUDES}
)

add_subdirectory(part)
add_subdirectory(plugins)
add_subdirectory(stencils)
add_subdirectory(templates)
add_subdirectory(pics)


########### install files ###############

install( FILES
    flow_dock.desktop
    DESTINATION ${SERVICETYPES_INSTALL_DIR})
