




########### next target ###############

set(rtfexport_PART_SRCS rtfexport.cc ExportFilter.cc )

kde4_automoc(${rtfexport_PART_SRCS})

kde4_add_plugin(rtfexport WITH_PREFIX ${rtfexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} rtfexport )

target_link_libraries(rtfexport  ${KDE4_KDECORE_LIBS} kwordexportfilters )

install(TARGETS rtfexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_rtf_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

