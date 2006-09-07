




########### next target ###############

set(rtfimport_PART_SRCS rtfimport.cpp rtfimport_dom.cpp rtfimport_tokenizer.cpp )

kde4_automoc(${rtfimport_PART_SRCS})

kde4_add_plugin(rtfimport WITH_PREFIX ${rtfimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} rtfimport )

target_link_libraries(rtfimport kofficecore)

install(TARGETS rtfimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_rtf_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
