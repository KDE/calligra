

include_directories(${CMAKE_SOURCE_DIR}/filters/liboofilter/)

set(libcommon_SRCS conversion.cc)



########### next target ###############

set(oowriterimport_PART_SRCS oowriterimport.cc ${libcommon_SRCS} ${liboofilter_SRCS})

kde4_add_plugin(oowriterimport WITH_PREFIX ${oowriterimport_PART_SRCS})



target_link_libraries(
    oowriterimport
    komain
    koodf
    kstore
    ${KDE4_KIO_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS oowriterimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(oowriterexport_PART_SRCS oowriterexport.cc ExportFilter.cc ${libcommon_SRCS} ${liboofilter_SRCS})

kde4_add_plugin(oowriterexport WITH_PREFIX ${oowriterexport_PART_SRCS})



target_link_libraries(
    oowriterexport
    komain
    kwordexportfilters
    kstore
    koodf
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS oowriterexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############


########### install files ###############

install( FILES  kword_oowriter_import.desktop kword_oowriter_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



