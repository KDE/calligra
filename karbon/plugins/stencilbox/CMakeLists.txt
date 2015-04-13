include_directories(${KOMAIN_INCLUDES} ${FLAKE_INCLUDES})

set(karbon_stencilbox_PART_SRCS
    StencilBoxPlugin.cpp
    StencilBoxDocker.cpp
    StencilListView.cpp
    CollectionTreeWidget.cpp
    CollectionItemModel.cpp
    StencilShapeFactory.cpp
)

add_library(karbon_stencilbox MODULE ${karbon_stencilbox_PART_SRCS})

kcoreaddons_desktop_to_json(karbon_stencilbox karbon_stencilbox.desktop)

target_link_libraries(karbon_stencilbox komain kowidgets flake KF5::Archive)

if(GHNS)
    target_link_libraries(karbon_stencilbox KF5::NewStuff)
endif()

install(TARGETS karbon_stencilbox DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

install(FILES karbon_stencils.knsrc DESTINATION ${CONFIG_INSTALL_DIR})
