set(wps2odt_PART_SRCS
    WPSImport.cpp
    ../../../outputhelper/StringDocumentHandler.cxx
    ../../../outputhelper/FemtoZip.cxx
    ../../../outputhelper/OutputFileHelper.cxx
)

add_library(calligra_filter_wps2odt MODULE ${wps2odt_PART_SRCS})

target_link_libraries(calligra_filter_wps2odt PRIVATE
    komain
    LibOdfGen::LibOdfGen
    LibRevenge::LibRevenge
    LibRevenge::LibRevengeStream
    LibWps::LibWps
)

install(TARGETS calligra_filter_wps2odt DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)
