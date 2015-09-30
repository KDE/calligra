project(flow)

add_definitions(-DTRANSLATION_DOMAIN=\"flow\")

include_directories(
    ${KOMAIN_INCLUDES}
)

add_subdirectory(part)
add_subdirectory(plugins)
add_subdirectory(stencils)
add_subdirectory(templates)
add_subdirectory(pics)
