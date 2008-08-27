
include_directories( ${CMAKE_SOURCE_DIR}/karbon
${CMAKE_SOURCE_DIR}/karbon/core ${CMAKE_SOURCE_DIR}/plugins/pathshapes ${KOGUIUTILS_INCLUDES} ${KDE4_INCLUDES}  )

########### next target ###############

set(karbonsvgexport_PART_SRCS 
    svgexport.cc
    ${SHAPES_SOURCES} 
)

kde4_add_plugin(karbonsvgexport WITH_PREFIX ${karbonsvgexport_PART_SRCS})

target_link_libraries(
    karbonsvgexport
    koguiutils
    karboncommon
    karbonui
    flake
    kstore
    komain
    ${KDE4_KDECORE_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS karbonsvgexport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############

set(karbonsvgimport_PART_SRCS 
    svgimport.cc
    SvgGradientHelper.cpp
    ${SHAPES_SOURCES} 
)

kde4_add_plugin(karbonsvgimport WITH_PREFIX ${karbonsvgimport_PART_SRCS})

target_link_libraries(
    karbonsvgimport
    koguiutils
    karboncommon
    karbonui
    kotext
    kstore
    komain
    flake
    ${KDE4_KDECORE_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS karbonsvgimport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  karbon_svg_export.desktop karbon_svg_import.desktop karbon_svgz_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})
