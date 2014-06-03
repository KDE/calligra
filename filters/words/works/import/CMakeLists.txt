include_directories(
    ../../../libodfhandler/
    ${LIBODFGEN_INCLUDE_DIRS}
    ${LIBWPS_INCLUDE_DIRS}
    ${KOMAIN_INCLUDES}
)

set(wps2odt_PART_SRCS
    WPSImport.cpp
    ../../../libodfhandler/DiskDocumentHandler.cxx
    ../../../libodfhandler/FemtoZip.cxx
    ../../../libodfhandler/OutputFileHelper.cxx
    ../../../libodfhandler/StdOutHandler.cxx
)

kde4_add_plugin(calligra_filter_wps2odt ${wps2odt_PART_SRCS})

target_link_libraries(calligra_filter_wps2odt komain ${LIBODFGEN_LIBRARIES} ${LIBWPS_LIBRARIES})

install(TARGETS calligra_filter_wps2odt DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES calligra_filter_wps2odt.desktop DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install(FILES words_wps_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
