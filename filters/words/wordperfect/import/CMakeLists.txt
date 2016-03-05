include_directories(
    ../../../outputhelper/
    ${LIBODFGEN_INCLUDE_DIRS}
    ${LIBREVENGE_INCLUDE_DIRS}
    ${LIBWPD_INCLUDE_DIRS}
    ${LIBWPG_INCLUDE_DIRS}
    ${KOMAIN_INCLUDES}
)

set(wpd2odt_PART_SRCS
    WPDImport.cpp
    ../../../outputhelper/StringDocumentHandler.cxx
    ../../../outputhelper/FemtoZip.cxx
    ../../../outputhelper/OutputFileHelper.cxx
)

add_library(calligra_filter_wpd2odt MODULE ${wpd2odt_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_wpd2odt calligra_filter_wpd2odt.desktop)

target_link_libraries(calligra_filter_wpd2odt
    komain ${LIBODFGEN_LIBRARIES} ${LIBREVENGE_LIBRARIES} ${LIBWPD_LIBRARIES} ${LIBWPG_LIBRARIES})

install(TARGETS calligra_filter_wpd2odt DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES words_wpd_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif()

if(SHOULD_BUILD_OKULAR_GENERATOR_WORDPERFECT)
    kcoreaddons_add_plugin(okularGenerator_wpd_calligra
        JSON libokularGenerator_wpd.json
        INSTALL_NAMESPACE "okular/generators"
        SOURCES OkularWpdGeneratorPlugin.cpp
    )

    target_link_libraries( okularGenerator_wpd_calligra
        kookularGenerator_odt
    )

    install( FILES okularWpd_calligra.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
    install( PROGRAMS okularApplication_wpd_calligra.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
endif()
