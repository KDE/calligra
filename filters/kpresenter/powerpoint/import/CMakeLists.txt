set(powerpointimport_PART_SRCS powerpointimport.cc datetimeformat.cc pictures.cc ${libppt_SRCS} )

kde4_add_plugin(powerpointimport WITH_PREFIX ${powerpointimport_PART_SRCS})

target_link_libraries(powerpointimport komain)

install(TARGETS powerpointimport  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  kpresenter_powerpoint_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

