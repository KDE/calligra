add_subdirectory(3rdparty)

include_directories(
    3rdparty/rtf-qt/src/
)

set(rtf2odt_PART_SRCS rtfimport.cpp )
add_library(calligra_filter_rtf2odt MODULE ${rtf2odt_PART_SRCS})
target_link_libraries(calligra_filter_rtf2odt RtfReader komain)

install(TARGETS calligra_filter_rtf2odt  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

#[[if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES  words_rtf_thumbnail.desktop  DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/thumbcreator)
endif()]]

if(SHOULD_BUILD_OKULAR_GENERATOR_RTF)
    kcoreaddons_add_plugin(okularGenerator_rtf_calligra
        INSTALL_NAMESPACE "okular/generators"
        SOURCES OkularRtfGeneratorPlugin.cpp
    )

    target_link_libraries( okularGenerator_rtf_calligra
        kookularGenerator_odt
    )

    install( PROGRAMS okularApplication_rtf_calligra.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
endif()
