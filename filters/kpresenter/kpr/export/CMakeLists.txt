
set(odp2kpr_PART_SRCS odp2kpr.cpp )

kde4_automoc(${odp2kpr_PART_SRCS})

kde4_add_plugin(odp2kpr WITH_PREFIX ${odp2kpr_PART_SRCS})

target_link_libraries(odp2kpr  ${KDE4_KDECORE_LIBS} kofficecore)

install(TARGETS odp2kpr  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  odp2kpr.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

