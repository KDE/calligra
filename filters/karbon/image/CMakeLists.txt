

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${KDE4_INCLUDES} )


########### next target ###############

set(karbonimageexport_PART_SRCS ImageExport.cpp ImageExportOptionsWidget.cpp )

kde4_add_ui_files( karbonimageexport_PART_SRCS ImageExportOptionsWidget.ui )

kde4_add_plugin(karbonimageexport ${karbonimageexport_PART_SRCS})

target_link_libraries(karbonimageexport karbonui komain)

install(TARGETS karbonimageexport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  karbon_png_export.desktop karbon_jpg_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
