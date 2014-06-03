include_directories(${KOMAIN_INCLUDES})

set(dbase2kspread_PART_SRCS dbaseimport.cc dbase.cpp )

kde4_add_plugin(calligra_filter_dbase2kspread ${dbase2kspread_PART_SRCS})

target_link_libraries(calligra_filter_dbase2kspread komain)

install(TARGETS calligra_filter_dbase2kspread  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  calligra_filter_dbase2kspread.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

