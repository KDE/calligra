

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${KDE4_INCLUDES} )


########### next target ###############

set(karbon2image_PART_SRCS ImageExport.cpp ImageExportOptionsWidget.cpp )

kde4_add_ui_files( karbon2image_PART_SRCS ImageExportOptionsWidget.ui )

kde4_add_plugin(calligra_filter_karbon2image ${karbon2image_PART_SRCS})

target_link_libraries(calligra_filter_karbon2image karbonui komain)

install(TARGETS calligra_filter_karbon2image  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  calligra_filter_karbon2png.desktop calligra_filter_karbon2jpg.desktop DESTINATION ${SERVICES_INSTALL_DIR})
