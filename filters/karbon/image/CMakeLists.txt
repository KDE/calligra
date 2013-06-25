

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${KDE4_INCLUDES} )


########### next target ###############

set(karbon2image_PART_SRCS ImageExport.cpp ImageExportOptionsWidget.cpp )

kde4_add_ui_files( calligra_filter_karbon2image ImageExportOptionsWidget.ui )

kde4_add_plugin(calligra_filter_karbon2image ${karbon2image_PART_SRCS})

target_link_libraries(calligra_filter_karbon2image karbonui komain)

install(TARGETS calligra_filter_karbon2image  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  karbon_png_export.desktop karbon_jpg_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
