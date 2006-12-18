
set(kpr2odp_PART_SRCS kprodp.cpp )

kde4_automoc(${kpr2odp_PART_SRCS})

kde4_add_plugin(kpr2odp WITH_PREFIX ${kpr2odp_PART_SRCS})

target_link_libraries(kpr2odp  ${KDE4_KDECORE_LIBS} kofficecore)

install(TARGETS kpr2odp  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kpr2odp.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

