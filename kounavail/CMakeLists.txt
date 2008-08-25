

include_directories( ${KOMAIN_INCLUDES} )


########### next target ###############

set(kounavailpart_PART_SRCS kounavail.cc )


kde4_add_plugin(kounavailpart WITH_PREFIX ${kounavailpart_PART_SRCS})



target_link_libraries(kounavailpart kstore koodf ${QT_QT3SUPPORT_LIBRARY} ${KDE4_KPARTS_LIBS} ${KDE4_KDECORE_LIBS} komain )

install(TARGETS kounavailpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kounavail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

