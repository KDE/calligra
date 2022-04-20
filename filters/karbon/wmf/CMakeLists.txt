
include_directories( ${VECTORIMAGE_INCLUDES} )


if(SHOULD_BUILD_FILTER_WMF_TO_SVG)

set(wmf2svg_PART_SRCS
    WmfImportDebug.cpp
    WmfImport.cpp
    WmfImportParser.cpp
)

add_library(calligra_filter_wmf2svg MODULE ${wmf2svg_PART_SRCS})
target_link_libraries(calligra_filter_wmf2svg kovectorimage komain koodf)

install(TARGETS calligra_filter_wmf2svg DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)
calligra_filter_desktop_to_json(calligra_filter_wmf2svg calligra_filter_wmf2svg.desktop)

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install(FILES  karbon_wmf_thumbnail.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
endif()

endif()


if(SHOULD_BUILD_FILTER_KARBON_TO_WMF)

set(karbon2wmf_PART_SRCS WmfExport.cpp)

add_library(calligra_filter_karbon2wmf MODULE ${karbon2wmf_PART_SRCS})
target_link_libraries(calligra_filter_karbon2wmf  kovectorimage karboncommon karbonui kopageapp)

install(TARGETS calligra_filter_karbon2wmf DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)
calligra_filter_desktop_to_json(calligra_filter_karbon2wmf calligra_filter_karbon2wmf.desktop)

endif()
