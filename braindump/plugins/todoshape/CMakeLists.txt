include_directories (${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${KOFFICE_INCLUDE_DIR} )

########### Flake Plugin library ###############

SET (todoshape_LIB_SRCS
    TodoShape.cpp
#     TodoShapeConfigWidget.cpp
    TodoShapeFactory.cpp
    TodoShapePlugin.cpp
)

# kde4_add_ui_files( todoshape_LIB_SRCS
#     TodoShapeConfigWidget.ui
# )

kde4_add_plugin(todoshape ${todoshape_LIB_SRCS})

target_link_libraries(
    todoshape
    ${KOMAIN_LIBRARY}
    flake
    kostore
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KFILE_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTWEBKIT_LIBRARY}
    )
install(TARGETS todoshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES todoshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )

add_subdirectory(data)
