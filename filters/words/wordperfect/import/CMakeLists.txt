include_directories(
    ../../../libodfhandler/
    ${LIBODFGEN_INCLUDE_DIRS}
    ${LIBWPD_INCLUDE_DIRS}
    ${LIBWPG_INCLUDE_DIRS}
    ${KOMAIN_INCLUDES}
)

set(wpd2odt_PART_SRCS
    WPDImport.cpp
    ../../../libodfhandler/DiskDocumentHandler.cxx
    ../../../libodfhandler/FemtoZip.cxx
    ../../../libodfhandler/OutputFileHelper.cxx
    ../../../libodfhandler/StdOutHandler.cxx
)

kde4_add_plugin(calligra_filter_wpd2odt ${wpd2odt_PART_SRCS})

target_link_libraries(calligra_filter_wpd2odt
    komain ${LIBODFGEN_LIBRARIES} ${LIBWPD_LIBRARIES} ${LIBWPG_LIBRARIES})

install(TARGETS calligra_filter_wpd2odt DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES calligra_filter_wpd2odt.desktop DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install( FILES words_wpd_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
