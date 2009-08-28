########### next target ###############
include_directories(${KDE4_INCLUDES} ${KOMAIN_INCLUDES})

set(applixspreadimport_PART_SRCS applixspreadimport.cc )

kde4_add_plugin(applixspreadimport WITH_PREFIX ${applixspreadimport_PART_SRCS})

target_link_libraries(applixspreadimport komain)

install(TARGETS applixspreadimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_applixspread_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})




