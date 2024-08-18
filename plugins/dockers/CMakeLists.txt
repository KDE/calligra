project(calligradockers)

add_definitions(-DTRANSLATION_DOMAIN=\"calligra-dockers\")

include_directories(
     ${FLAKE_INCLUDES}
     ${KOWIDGETS_INCLUDES}     
)

set(calligradockers_SRCS

    snapguidedocker/SnapGuideDocker.cpp
    snapguidedocker/SnapGuideDockerFactory.cpp

    Plugin.cpp
)

add_library(calligra_docker_defaults MODULE ${calligradockers_SRCS})

target_link_libraries(calligra_docker_defaults kowidgets)

install(TARGETS calligra_docker_defaults DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/dockers)
