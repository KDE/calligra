
include_directories( ${KWMF_INCLUDES} ${CMAKE_SOURCE_DIR}/shapes/ )

########### next target ###############

set(wmfimport_PART_SRCS 
    wmfimport.cc 
    wmfimportparser.cc
    ${SHAPES_SOURCES} )

kde4_add_plugin(wmfimport WITH_PREFIX ${wmfimport_PART_SRCS})

target_link_libraries(wmfimport kowmf karbonui)

install(TARGETS wmfimport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############

set(wmfexport_PART_SRCS wmfexport.cc )

kde4_add_plugin(wmfexport WITH_PREFIX ${wmfexport_PART_SRCS})

target_link_libraries(
    wmfexport
    kowmf
    karboncommon
    karbonui
    pigmentcms
    komain
    flake
    ${KDE4_KDECORE_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS wmfexport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  karbon_wmf_import.desktop karbon_wmf_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

