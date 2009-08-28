

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core  ${KDE4_INCLUDES}  )


########### next target ###############

set(karbonpngexport_PART_SRCS PngExport.cpp PngExportOptionsWidget.cpp )

kde4_add_ui_files( karbonpngexport_PART_SRCS PngExportOptionsWidget.ui )

kde4_add_plugin(karbonpngexport WITH_PREFIX ${karbonpngexport_PART_SRCS})

target_link_libraries(karbonpngexport karbonui koinput komain koaspectbutton)

install(TARGETS karbonpngexport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  karbon_png_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
