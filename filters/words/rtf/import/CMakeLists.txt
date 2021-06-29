add_subdirectory(3rdparty)

include_directories(
    3rdparty/rtf-qt/src/
)

set(rtf2odt_PART_SRCS rtfimport.cpp )
add_library(calligra_filter_rtf2odt MODULE ${rtf2odt_PART_SRCS})
calligra_filter_desktop_to_json(calligra_filter_rtf2odt calligra_filter_rtf2odt.desktop)
target_link_libraries(calligra_filter_rtf2odt RtfReader komain)

install(TARGETS calligra_filter_rtf2odt  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES  words_rtf_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif()

if(SHOULD_BUILD_OKULAR_GENERATOR_RTF)
    kcoreaddons_add_plugin(okularGenerator_rtf_calligra
        INSTALL_NAMESPACE "okular/generators"
        SOURCES OkularRtfGeneratorPlugin.cpp
    )

    target_link_libraries( okularGenerator_rtf_calligra
        kookularGenerator_odt
    )

    install( FILES okularRtf_calligra.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
    install( PROGRAMS okularApplication_rtf_calligra.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
endif()
