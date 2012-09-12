
include_directories( ${KWMF_INCLUDES} )

########### next target ###############

set(wmfimport_PART_SRCS 
    WmfImport.cpp 
    WmfImportParser.cpp
)

kde4_add_plugin(wmfimport ${wmfimport_PART_SRCS})

target_link_libraries(wmfimport libwmf komain koodf)

install(TARGETS wmfimport
        DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############

set(wmfexport_PART_SRCS WmfExport.cpp)

kde4_add_plugin(wmfexport ${wmfexport_PART_SRCS})

target_link_libraries(
    wmfexport
    libwmf
    karboncommon
    karbonui
)

install(TARGETS wmfexport
        DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES  karbon_wmf_import.desktop karbon_wmf_export.desktop karbon_wmf_thumbnail.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

