include_directories(
    ${QT_INCLUDES} 
    ${KDE4_INCLUDE_DIR}
    ${FLAKE_INCLUDES}
    ${CMAKE_SOURCE_DIR}/krita 
    ${CMAKE_SOURCE_DIR}/krita/ui
    ${CMAKE_SOURCE_DIR}/krita/ui/widgets 
)

########### next target ###############

set(kritavectortools_PART_SRCS
   KritaVectorToolsPlugin.cpp
   KritaBlobTool.cpp
   KritaBlobToolFactory.cpp
)

kde4_add_ui_files(kritavectortools_PART_SRCS

)

kde4_add_plugin(kritavectortools ${kritavectortools_PART_SRCS})

target_link_libraries(kritavectortools kritaui kowidgets)

install(TARGETS kritavectortools DESTINATION ${PLUGIN_INSTALL_DIR})

kde4_install_icons( ${DATA_INSTALL_DIR}/krita/icons  )

########### install files ###############

install( FILES       blob_tool.png
         DESTINATION ${DATA_INSTALL_DIR}/krita/pics  )
install( FILES kritavectortools.desktop  DESTINATION ${SERVICES_INSTALL_DIR} )
