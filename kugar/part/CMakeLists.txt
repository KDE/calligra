

include_directories(${CMAKE_SOURCE_DIR}/kugar/lib/)


########### next target ###############

set(kugarpart_PART_SRCS kugar_part.cpp kugar_factory.cpp kugar_view.cpp )

kde4_automoc(${kugarpart_PART_SRCS})

kde4_add_plugin(kugarpart WITH_PREFIX ${kugarpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kugarpart )

target_link_libraries(kugarpart  ${KDE4_KIO_LIBS} kugarlib ${KDE4_KPARTS_LIBS} ${KDE4_KDEPRINT_LIBS} kofficecore )

install(TARGETS kugarpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kugar_KDEINIT_SRCS main.cpp )

kde4_automoc(${kugar_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kugar ${kugar_KDEINIT_SRCS})

target_link_libraries(kdeinit_kugar  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kdeinit_kugar  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kugar kdeinit_kugar )
install(TARGETS kugar  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  kugar.desktop  DESTINATION ${XDG_APPS_DIR})
install( FILES  kugarpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kugarpart.rc  DESTINATION ${DATA_INSTALL_DIR}/kugar)
install( FILES  kugartemplate.dtd  DESTINATION ${DATA_INSTALL_DIR}/kugar/dtd)


