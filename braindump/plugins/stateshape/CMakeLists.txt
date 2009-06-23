include_directories (${CMAKE_CURRENT_BINARY_DIR} ${KOFFICE_INCLUDE_DIR} )

########### Flake Plugin library ###############

SET (stateshape_LIB_SRCS
    CategorizedItemDelegate.cpp
    StatesModel.cpp
    StatesRegistry.cpp
    StateShape.cpp
    StateShapeChangeStateCommand.cpp
    StateShapeFactory.cpp
    StateShapePlugin.cpp
    StateTool.cpp
    StateToolFactory.cpp
    StateToolWidget.cpp
)

kde4_add_ui_files( stateshape_LIB_SRCS
    StateShapeConfigWidget.ui
)

kde4_add_plugin(stateshape ${stateshape_LIB_SRCS})

target_link_libraries(
    stateshape
    ${KOMAIN_LIBRARY}
    flake
    kostore
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KFILE_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    )
install(TARGETS stateshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES stateshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )

add_subdirectory(data)
