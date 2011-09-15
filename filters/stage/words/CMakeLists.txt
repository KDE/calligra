########### odp 2 odt filter ###############

set(kodp2odt_PART_SRCS odp2odt.cpp )

kde4_add_plugin(kodp2odt ${kodp2odt_PART_SRCS})



target_link_libraries(kodp2odt komain)

install(TARGETS kodp2odt  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kodp2odt.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

########### older kpr 2 kwd filter ###############

set(kprwords_PART_SRCS kprwords.cc )


kde4_add_plugin(kprwords ${kprwords_PART_SRCS})



target_link_libraries(kprwords  ${KDE4_KDECORE_LIBS} komain)

install(TARGETS kprwords  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kprwords.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

