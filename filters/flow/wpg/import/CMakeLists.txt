include_directories(
    ../../../libodfhandler/
    ${LIBODFGEN_INCLUDE_DIRS}
    ${LIBVISIO_INCLUDE_DIRS}
    ${KOMAIN_INCLUDES}
)

set(wpg2odg_PART_SRCS
    WPGImport.cpp
    ../../../libodfhandler/DiskDocumentHandler.cxx
    ../../../libodfhandler/FemtoZip.cxx
    ../../../libodfhandler/OutputFileHelper.cxx
    ../../../libodfhandler/StdOutHandler.cxx
)

kde4_add_plugin(calligra_filter_wpg2odg ${wpg2odg_PART_SRCS})

target_link_libraries(calligra_filter_wpg2odg komain ${LIBODFGEN_LIBRARIES} ${LIBWPG_LIBRARIES})

install(TARGETS calligra_filter_wpg2odg DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES calligra_filter_wpg2odg.desktop DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install(FILES flow_wpg_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
