project(flowdockersplugin)

include_directories(${KDE4_INCLUDE_DIR}
     ${FLAKE_INCLUDES}
     ${KOPAGEAPP_INCLUDES}
)

set(flowdockers_SRCS
    Plugin.cpp
    
    stencilboxdocker/StencilBoxDocker.cpp
    stencilboxdocker/StencilBoxView.cpp
    stencilboxdocker/CollectionItemModel.cpp
    stencilboxdocker/StencilShapeFactory.cpp
)

kde4_add_plugin(flowdockersplugin ${flowdockers_SRCS})

target_link_libraries(flowdockersplugin komain kowidgets ${KDE4_KNEWSTUFF3_LIBS})

install(TARGETS flowdockersplugin DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES flowdockersplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES stencilboxdocker/flow_stencils.knsrc DESTINATION ${CONFIG_INSTALL_DIR})
