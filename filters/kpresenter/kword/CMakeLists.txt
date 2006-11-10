########### odp 2 odt filter ###############

set(kodp2odt_PART_SRCS odp2odt.cpp )

kde4_automoc(${kodp2odt_PART_SRCS})

kde4_add_plugin(kodp2odt WITH_PREFIX ${kodp2odt_PART_SRCS})



target_link_libraries(kodp2odt  ${KDE4_KDECORE_LIBS} kofficecore)

install(TARGETS kodp2odt  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kodp2odt.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

########### older kpr 2 kwd filter ###############

set(kprkword_PART_SRCS kprkword.cc )

kde4_automoc(${kprkword_PART_SRCS})

kde4_add_plugin(kprkword WITH_PREFIX ${kprkword_PART_SRCS})



target_link_libraries(kprkword  ${KDE4_KDECORE_LIBS} kofficecore)

install(TARGETS kprkword  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kprkword.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

