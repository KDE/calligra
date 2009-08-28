
set(kpr2odp_PART_SRCS kpr2odp.cpp )


kde4_add_plugin(kpr2odp WITH_PREFIX ${kpr2odp_PART_SRCS})

target_link_libraries(kpr2odp komain)

install(TARGETS kpr2odp  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kpr2odp.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

