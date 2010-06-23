set(dcmimport_PART_SRCS dcmimport.cc DCMImportDialog.cc )
kde4_add_ui_files(dcmimport_PART_SRCS DCMImportDialog.ui )

INCLUDE_DIRECTORIES( ${DCMTK_INCLUDE_DIR} )
add_definitions(-DHAVE_CONFIG_H)

kde4_add_plugin(dcmimport WITH_PREFIX ${dcmimport_PART_SRCS})

target_link_libraries(dcmimport kwordexportfilters ${DCMTK_LIBRARIES})

install(TARGETS dcmimport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kword_dcm_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
