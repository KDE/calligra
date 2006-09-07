

include_directories(${KOFFICEUI_INCLUDES} ${KOTEXT_INCLUDES})


########### next target ###############

set(htmlimport_PART_SRCS htmlimport.cpp khtmlreader.cpp kwdwriter.cpp )

kde4_automoc(${htmlimport_PART_SRCS})

kde4_add_plugin(htmlimport WITH_PREFIX ${htmlimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} htmlimport )

target_link_libraries(htmlimport  ${KDE4_KDECORE_LIBS} kotext ${KDE4_KHTML_LIBS} )

install(TARGETS htmlimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_html_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


