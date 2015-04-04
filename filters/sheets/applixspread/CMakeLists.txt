########### next target ###############
include_directories( ${KOMAIN_INCLUDES})

set(applixspread2kspread_PART_SRCS applixspreadimport.cc )

add_library(calligra_filter_applixspread2kspread MODULE ${applixspread2kspread_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_applixspread2kspread calligra_filter_applixspread2kspread.desktop)

target_link_libraries(calligra_filter_applixspread2kspread komain calligrasheetscommon)

install(TARGETS calligra_filter_applixspread2kspread  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
