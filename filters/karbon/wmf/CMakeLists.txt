
include_directories( ${VECTORIMAGE_INCLUDES} )


if(SHOULD_BUILD_FILTER_WMF_TO_SVG)

set(wmfimport_PART_SRCS 
    WmfImport.cpp 
    WmfImportParser.cpp
)

kde4_add_plugin(wmfimport ${wmfimport_PART_SRCS})

target_link_libraries(wmfimport vectorimage komain koodf)

install(TARGETS wmfimport
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES  karbon_wmf_import.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install(FILES  karbon_wmf_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)

endif(SHOULD_BUILD_FILTER_WMF_TO_SVG)


if(SHOULD_BUILD_FILTER_KARBON_TO_WMF)

set(wmfexport_PART_SRCS WmfExport.cpp)

kde4_add_plugin(wmfexport ${wmfexport_PART_SRCS})

target_link_libraries(
    wmfexport
    vectorimage
    karboncommon
    karbonui
)

install(TARGETS wmfexport
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES  karbon_wmf_export.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_KARBON_TO_WMF)
