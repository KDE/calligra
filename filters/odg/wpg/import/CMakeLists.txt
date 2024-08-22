include_directories(
    ../../../outputhelper/
    ${LIBWPG_INCLUDE_DIRS}
    ${KOMAIN_INCLUDES}
)

set(wpg2odg_PART_SRCS
    WPGImport.cpp
    ../../../outputhelper/StringDocumentHandler.cxx
    ../../../outputhelper/FemtoZip.cxx
    ../../../outputhelper/OutputFileHelper.cxx
)

add_library(calligra_filter_wpg2odg MODULE ${wpg2odg_PART_SRCS})

target_link_libraries(calligra_filter_wpg2odg PRIVATE
    komain
    LibOdfGen::LibOdfGen
    LibRevenge::LibRevenge
    LibRevenge::LibRevengeStream
    ${LIBWPG_LIBRARIES}
)

install(TARGETS calligra_filter_wpg2odg DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

