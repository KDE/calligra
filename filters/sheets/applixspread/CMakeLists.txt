########### next target ###############
include_directories(${KDE4_INCLUDES} ${KOMAIN_INCLUDES})

set(applixspread2kspread_PART_SRCS applixspreadimport.cc )

kde4_add_plugin(calligra_filter_applixspread2kspread ${applixspread2kspread_PART_SRCS})

target_link_libraries(calligra_filter_applixspread2kspread komain calligrasheetscommon)

install(TARGETS calligra_filter_applixspread2kspread  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  calligra_filter_applixspread2kspread.desktop  DESTINATION ${SERVICES_INSTALL_DIR})




