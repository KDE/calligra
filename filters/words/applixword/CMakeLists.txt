########### next target ###############

set(applixword2odt_PART_SRCS applixwordimport.cc )


add_library(calligra_filter_applixword2odt MODULE ${applixword2odt_PART_SRCS})


target_link_libraries(calligra_filter_applixword2odt komain)

install(TARGETS calligra_filter_applixword2odt  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)
