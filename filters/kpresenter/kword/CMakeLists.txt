




########### next target ###############

set(kprkword_PART_SRCS kprkword.cc )

kde4_automoc(${kprkword_PART_SRCS})

kde4_add_plugin(kprkword WITH_PREFIX ${kprkword_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kprkword )

target_link_libraries(kprkword  ${KDE4_KDECORE_LIBS} kofficecore)

install(TARGETS kprkword  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kprkword.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

