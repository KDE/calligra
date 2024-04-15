include_directories(${KOMAIN_INCLUDES})

set(dbase2kspread_PART_SRCS dbaseimport.cc dbase.cpp )

add_library(calligra_filter_dbase2kspread MODULE ${dbase2kspread_PART_SRCS})

target_link_libraries(calligra_filter_dbase2kspread komain)

install(TARGETS calligra_filter_dbase2kspread  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)


