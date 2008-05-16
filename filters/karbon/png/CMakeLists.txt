

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KOGUIUTILS_INCLUDES} ${KDE4_INCLUDES}  )


########### next target ###############

set(karbonpngexport_PART_SRCS pngexport.cc PngExportOptionsWidget.cpp )

kde4_add_ui_files( karbonpngexport_PART_SRCS PngExportOptionsWidget.ui )

kde4_add_plugin(karbonpngexport WITH_PREFIX ${karbonpngexport_PART_SRCS})

target_link_libraries(karbonpngexport  ${KDE4_KDECORE_LIBS} koguiutils karboncommon karbonui )

install(TARGETS karbonpngexport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  karbon_png_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
