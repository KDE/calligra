include_directories(${FLAKE_INCLUDES})

SET ( PresentationViewPortShape_SRCS
    PresentationViewPortShape.cpp
    PresentationViewPortShapeFactory.cpp
    PresentationViewPortShapePlugin.cpp
    PresentationViewPortTool.cpp
    PresentationViewPortToolFactory.cpp
    PresentationViewPortConfigWidget.cpp 
    PresentationViewPortShapeUtil.cpp
    )

kde4_add_ui_files( PresentationViewPortShape_SRCS
    PresentationViewPortConfigWidget.ui
    )

kde4_add_plugin(presentationviewportshape ${PresentationViewPortShape_SRCS})

target_link_libraries(presentationviewportshape flake)
install(TARGETS presentationviewportshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES presentationviewportshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})


