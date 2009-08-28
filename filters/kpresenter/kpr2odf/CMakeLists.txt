set(Filterkpr2odf_PART_SRCS Filterkpr2odf.cpp )

kde4_add_plugin(Filterkpr2odf WITH_PREFIX ${Filterkpr2odf_PART_SRCS})

target_link_libraries(Filterkpr2odf komain)

install(TARGETS Filterkpr2odf DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  Filterkpr2odf.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
