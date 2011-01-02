include_directories (${CMAKE_CURRENT_BINARY_DIR} )

########### Flake Plugin library ###############

SET (comicboxesshape_LIB_SRCS
    ComicBoxesLine.cpp
    ComicBoxesShape.cpp
    ComicBoxesShapeFactory.cpp
    ComicBoxesPlugin.cpp
    ComicBoxesTool.cpp
    ComicBoxesToolFactory.cpp
    ComicBoxesCreationWidget.cpp
    Curve.cpp
)

kde4_add_ui_files( comicboxesshape_LIB_SRCS
    ComicBoxesCreationWidget.ui
)

kde4_add_plugin(comicboxesshape ${comicboxesshape_LIB_SRCS})

target_link_libraries(comicboxesshape komain )
install(TARGETS comicboxesshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES comicboxesshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
