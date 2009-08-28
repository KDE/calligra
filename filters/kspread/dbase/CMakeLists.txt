include_directories(${KOMAIN_INCLUDES})

set(dbaseimport_PART_SRCS dbaseimport.cc dbase.cpp )

kde4_add_plugin(dbaseimport WITH_PREFIX ${dbaseimport_PART_SRCS})

target_link_libraries(dbaseimport komain)

install(TARGETS dbaseimport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kspread_dbase_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

