########### next target ###############

set(applixword2odt_PART_SRCS applixwordimport.cc )


add_library(calligra_filter_applixword2odt MODULE ${applixword2odt_PART_SRCS})

kcoreaddons_desktop_to_json(calligra_filter_applixword2odt calligra_filter_applixword2odt.desktop)

target_link_libraries(calligra_filter_applixword2odt komain)

install(TARGETS calligra_filter_applixword2odt  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
