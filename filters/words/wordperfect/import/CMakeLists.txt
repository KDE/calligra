include_directories(
    ../../../outputhelper/
    ${KOMAIN_INCLUDES}
)

set(wpd2odt_PART_SRCS
    WPDImport.cpp
    ../../../outputhelper/StringDocumentHandler.cxx
    ../../../outputhelper/FemtoZip.cxx
    ../../../outputhelper/OutputFileHelper.cxx
)

add_library(calligra_filter_wpd2odt MODULE ${wpd2odt_PART_SRCS})

target_link_libraries(calligra_filter_wpd2odt PRIVATE
    komain
    LibOdfGen::LibOdfGen
    LibWpg::LibWpg
    LibRevenge::LibRevenge
    LibRevenge::LibRevengeStream
    LibWpd::LibWpd
)

install(TARGETS calligra_filter_wpd2odt DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

if(SHOULD_BUILD_OKULAR_GENERATOR_WORDPERFECT)
    kcoreaddons_add_plugin(okularGenerator_wpd_calligra
        INSTALL_NAMESPACE "okular_generators"
        SOURCES OkularWpdGeneratorPlugin.cpp
    )

    target_link_libraries( okularGenerator_wpd_calligra
        kookularGenerator_odt
    )

    install( PROGRAMS okularApplication_wpd_calligra.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
endif()
