include_directories(${KOMAIN_INCLUDES})

set(dbase2kspread_PART_SRCS dbaseimport.cc dbase.cpp )

add_library(calligra_filter_dbase2kspread MODULE ${dbase2kspread_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_dbase2kspread calligra_filter_dbase2kspread.desktop)

target_link_libraries(calligra_filter_dbase2kspread komain)

install(TARGETS calligra_filter_dbase2kspread  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


