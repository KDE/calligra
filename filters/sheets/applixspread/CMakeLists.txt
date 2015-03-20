########### next target ###############
include_directories( ${KOMAIN_INCLUDES})

set(applixspread2kspread_PART_SRCS applixspreadimport.cc )

add_library(calligra_filter_applixspread2kspread MODULE ${applixspread2kspread_PART_SRCS})

target_link_libraries(calligra_filter_applixspread2kspread komain calligrasheetscommon)

install(TARGETS calligra_filter_applixspread2kspread  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  calligra_filter_applixspread2kspread.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)




