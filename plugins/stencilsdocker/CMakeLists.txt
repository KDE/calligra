include_directories(${KOMAIN_INCLUDES} ${FLAKE_INCLUDES})

set(stencilsdocker_PART_SRCS
    StencilBoxDebug.cpp
    StencilBoxPlugin.cpp
    StencilBoxDocker.cpp
    StencilListView.cpp
    CollectionTreeWidget.cpp
    CollectionItemModel.cpp
    StencilShapeFactory.cpp
)

add_library(calligra_docker_stencils MODULE ${stencilsdocker_PART_SRCS})

calligra_docker_desktop_to_json(calligra_docker_stencils calligra_docker_stencils.desktop)

target_link_libraries(calligra_docker_stencils komain kowidgets flake KF5::Archive KF5::IconThemes)

install(TARGETS calligra_docker_stencils DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/dockers)

add_subdirectory(stencils)
