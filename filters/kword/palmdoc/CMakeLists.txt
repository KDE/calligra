




########### next target ###############

set(palmdocimport_PART_SRCS palmdocimport.cc palmdb.cpp palmdoc.cpp )

kde4_automoc(${palmdocimport_PART_SRCS})

kde4_add_plugin(palmdocimport WITH_PREFIX ${palmdocimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} palmdocimport )

target_link_libraries(palmdocimport kofficecore)

install(TARGETS palmdocimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(palmdocexport_PART_SRCS palmdocexport.cc palmdb.cpp palmdoc.cpp )

kde4_automoc(${palmdocexport_PART_SRCS})

kde4_add_plugin(palmdocexport WITH_PREFIX ${palmdocexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} palmdocexport )

target_link_libraries(palmdocexport kofficecore kwordexportfilters)

install(TARGETS palmdocexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_palmdoc_import.desktop kword_palmdoc_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



