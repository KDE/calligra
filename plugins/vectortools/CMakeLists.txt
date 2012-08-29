project ( calligravectortools )

include_directories(
    ${QT_INCLUDES} 
    ${KDE4_INCLUDE_DIR}
    ${FLAKE_INCLUDES}
)

########### next target ###############

set(calligravectortools_PART_SRCS
   KoVectorToolsPlugin.cpp
   KoBlobTool.cpp
   KoBlobToolFactory.cpp
)

kde4_add_ui_files(calligravectortools_PART_SRCS

)

kde4_add_plugin(calligravectortools ${calligravectortools_PART_SRCS})

target_link_libraries(calligravectortools)

install(TARGETS calligravectortools DESTINATION ${PLUGIN_INSTALL_DIR})

kde4_install_icons( ${DATA_INSTALL_DIR}/icons  )

########### install files ###############

install( FILES       blob_tool.png
         DESTINATION ${DATA_INSTALL_DIR}/pics  )
install( FILES calligravectortools.desktop  DESTINATION ${SERVICES_INSTALL_DIR} )
