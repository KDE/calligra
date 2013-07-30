
include_directories( ${VECTORIMAGE_INCLUDES} )


if(SHOULD_BUILD_FILTER_WMF_TO_SVG)

set(wmf2svg_PART_SRCS
    WmfImport.cpp
    WmfImportParser.cpp
)

kde4_add_plugin(calligra_filter_wmf2svg ${wmf2svg_PART_SRCS})

target_link_libraries(calligra_filter_wmf2svg kovectorimage komain koodf)

install(TARGETS calligra_filter_wmf2svg
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES  karbon_wmf_import.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install(FILES  karbon_wmf_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)

endif(SHOULD_BUILD_FILTER_WMF_TO_SVG)


if(SHOULD_BUILD_FILTER_KARBON_TO_WMF)

set(karbon2wmf_PART_SRCS WmfExport.cpp)

kde4_add_plugin(calligra_filter_karbon2wmf ${karbon2wmf_PART_SRCS})

target_link_libraries(
    calligra_filter_karbon2wmf
    kovectorimage
    karboncommon
    karbonui
)

install(TARGETS calligra_filter_karbon2wmf
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES  karbon_wmf_export.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_KARBON_TO_WMF)
