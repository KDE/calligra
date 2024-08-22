include_directories(
    ../../../outputhelper/
    ${KOMAIN_INCLUDES}
)

set(vsdx2odg_PART_SRCS
    VSDXImport.cpp
    ../../../outputhelper/StringDocumentHandler.cxx
    ../../../outputhelper/FemtoZip.cxx
    ../../../outputhelper/OutputFileHelper.cxx
)

add_library(calligra_filter_vsdx2odg MODULE ${vsdx2odg_PART_SRCS})

target_link_libraries(calligra_filter_vsdx2odg
    komain
    LibOdfGen::LibOdfGen
    LibRevenge::LibRevenge
    LibRevenge::LibRevengeStream
    LibVisio::LibVisio
)

install(TARGETS calligra_filter_vsdx2odg DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

