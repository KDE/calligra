include_directories(
    ${WRITERPERFECT_INCLUDES}
    ${LIBVISIO_INCLUDE_DIR}
    ${WPD_INCLUDE_DIR}
    ${LIBWPG_INCLUDE_DIR}
    ${KOTEXT_INCLUDES}
    ${KOMAIN_INCLUDES}
    ${FLAKE_INCLUDES}
)

set(vsdximport_PART_SRCS VSDXImport.cpp)

kde4_add_plugin(vsdximport ${vsdximport_PART_SRCS})

target_link_libraries(vsdximport komain writerperfect ${LIBVISIO_LIBRARIES})

install(TARGETS vsdximport DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES flow_vsdx_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install(FILES flow_vsdx_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
