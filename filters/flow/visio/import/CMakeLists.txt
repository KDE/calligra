include_directories(
    ${LIBVISIO_INCLUDE_DIR}
    ${WPD_INCLUDE_DIR}
    ${LIBWPG_INCLUDE_DIR}
)

set(vsdximport_PART_SRCS
	DocumentElement.cxx
	TextRunStyle.cxx
	FontStyle.cxx
        OdgGenerator.cxx
        OutputHandler.cpp
	VSDXImport.cpp
)

kde4_add_plugin(vsdximport ${vsdximport_PART_SRCS})

target_link_libraries(vsdximport komain ${LIBVISIO_LIBRARIES})

install(TARGETS vsdximport DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES flow_vsdx_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})

