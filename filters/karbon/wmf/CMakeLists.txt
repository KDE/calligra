
include_directories( ${VECTORIMAGE_INCLUDES} )


if(SHOULD_BUILD_FILTER_WMF_TO_SVG)

set(wmf2svg_PART_SRCS
    WmfImport.cpp
    WmfImportParser.cpp
)

add_library(calligra_filter_wmf2svg MODULE ${wmf2svg_PART_SRCS})
target_link_libraries(calligra_filter_wmf2svg kovectorimage komain koodf)

install(TARGETS calligra_filter_wmf2svg DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
kcoreaddons_desktop_to_json(calligra_filter_wmf2svg calligra_filter_wmf2svg.desktop)

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install(FILES  karbon_wmf_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif()

endif()


if(SHOULD_BUILD_FILTER_KARBON_TO_WMF)

set(karbon2wmf_PART_SRCS WmfExport.cpp)

add_library(calligra_filter_karbon2wmf MODULE ${karbon2wmf_PART_SRCS})
target_link_libraries(calligra_filter_karbon2wmf  kovectorimage karboncommon karbonui)

install(TARGETS calligra_filter_karbon2wmf DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
kcoreaddons_desktop_to_json(calligra_filter_karbon2wmf calligra_filter_karbon2wmf.desktop)

endif()
