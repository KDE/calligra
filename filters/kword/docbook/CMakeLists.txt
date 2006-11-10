




########### next target ###############

set(docbookexport_PART_SRCS docbookexport.cc )

kde4_automoc(${docbookexport_PART_SRCS})

kde4_add_plugin(docbookexport WITH_PREFIX ${docbookexport_PART_SRCS})



target_link_libraries(docbookexport  ${KDE4_KDECORE_LIBS} kwordexportfilters)

install(TARGETS docbookexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_docbook_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

