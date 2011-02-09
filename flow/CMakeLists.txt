
project(flow)
include_directories(
    ${KOMAIN_INCLUDES}
)

add_subdirectory(src)
add_subdirectory(templates)
add_subdirectory(stencils)

########### install files ###############

install( FILES
    flow_dock.desktop
    DESTINATION ${SERVICETYPES_INSTALL_DIR})
