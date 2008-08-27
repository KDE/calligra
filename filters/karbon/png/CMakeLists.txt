

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KOGUIUTILS_INCLUDES} ${KDE4_INCLUDES}  )


########### next target ###############

set(karbonpngexport_PART_SRCS PngExport.cpp PngExportOptionsWidget.cpp )

kde4_add_ui_files( karbonpngexport_PART_SRCS PngExportOptionsWidget.ui )

kde4_add_plugin(karbonpngexport WITH_PREFIX ${karbonpngexport_PART_SRCS})

target_link_libraries(
    karbonpngexport
    koguiutils
    karboncommon
    karbonui
    komain
    koodf
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS karbonpngexport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  karbon_png_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
