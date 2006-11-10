include_directories(${CMAKE_SOURCE_DIR}/filters/liboofilter/)




########### next target ###############

set(ooimpressimport_PART_SRCS ooimpressimport.cc ${liboofilter_SRCS})

kde4_automoc(${ooimpressimport_PART_SRCS})

kde4_add_plugin(ooimpressimport WITH_PREFIX ${ooimpressimport_PART_SRCS})



target_link_libraries(ooimpressimport  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS ooimpressimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(ooimpressexport_PART_SRCS ooimpressexport.cc stylefactory.cc ${liboofilter_SRCS})

kde4_automoc(${ooimpressexport_PART_SRCS})

kde4_add_plugin(ooimpressexport WITH_PREFIX ${ooimpressexport_PART_SRCS})



target_link_libraries(ooimpressexport  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS ooimpressexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kpresenter_ooimpress_import.desktop kpresenter_ooimpress_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

