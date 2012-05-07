include_directories(${CMAKE_SOURCE_DIR}/filters/liboofilter/)




########### next target ###############

set(ooimpressimport_PART_SRCS ooimpressimport.cc ${liboofilter_SRCS})


kde4_add_plugin(ooimpressimport ${ooimpressimport_PART_SRCS})



target_link_libraries(ooimpressimport ${KOMAIN_LIBRARY})

install(TARGETS ooimpressimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(ooimpressexport_PART_SRCS ooimpressexport.cc stylefactory.cc ${liboofilter_SRCS})

kde4_add_plugin(ooimpressexport ${ooimpressexport_PART_SRCS})



target_link_libraries(ooimpressexport  ${KDE4_KDECORE_LIBS} ${KOMAIN_LIBRARY} )

install(TARGETS ooimpressexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kpresenter_ooimpress_import.desktop kpresenter_ooimpress_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

