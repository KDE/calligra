########### next target ###############
include_directories( ${KOMAIN_INCLUDES})

set(applixspread2kspread_PART_SRCS applixspreadimport.cc )

add_library(calligra_filter_applixspread2kspread MODULE ${applixspread2kspread_PART_SRCS})

target_link_libraries(calligra_filter_applixspread2kspread komain calligrasheetsengine)

install(TARGETS calligra_filter_applixspread2kspread  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)
