include_directories(
    ../../../outputhelper/
    ${LIBODFGEN_INCLUDE_DIRS}
    ${LIBREVENGE_INCLUDE_DIRS}
    ${LIBVISIO_INCLUDE_DIRS}
    ${KOMAIN_INCLUDES}
)

set(vsdx2odg_PART_SRCS
    VSDXImport.cpp
    ../../../outputhelper/StringDocumentHandler.cxx
    ../../../outputhelper/FemtoZip.cxx
    ../../../outputhelper/OutputFileHelper.cxx
)

add_library(calligra_filter_vsdx2odg MODULE ${vsdx2odg_PART_SRCS})

target_link_libraries(calligra_filter_vsdx2odg komain ${LIBODFGEN_LIBRARIES} ${LIBREVENGE_LIBRARIES} ${LIBVISIO_LIBRARIES})

install(TARGETS calligra_filter_vsdx2odg DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

