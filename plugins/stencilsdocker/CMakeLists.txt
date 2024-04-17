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


target_link_libraries(calligra_docker_stencils komain kowidgets flake KF6::Archive KF6::IconThemes)

install(TARGETS calligra_docker_stencils DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/dockers)

add_subdirectory(stencils)
