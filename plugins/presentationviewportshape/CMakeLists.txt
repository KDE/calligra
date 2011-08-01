include_directories(${FLAKE_INCLUDES} ${KOMAIN_INCLUDES} )

SET (presentationviewportshape_LIB_SRCS
    PresentationViewPortShape.cpp
    PresentationViewPortShapeFactory.cpp
    PresentationViewPortShapePlugin.cpp
    PresentationViewPortTool.cpp
    PresentationViewPortToolFactory.cpp
    PresentationViewPortConfigWidget.cpp 
    )

kde4_add_ui_files( presentationviewportshape_LIB_SRCS
    PresentationViewPortConfigWidget.ui
    )

kde4_add_plugin(presentationviewportshape ${presentationviewportshape_LIB_SRCS})

target_link_libraries(presentationviewportshape flake)
install(TARGETS presentationviewportshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES presentationviewportshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )

