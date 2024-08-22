include_directories(
    ../../../outputhelper/
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

target_link_libraries(calligra_filter_wpd2odt PRIVATE
    komain
    LibOdfGen::LibOdfGen
    LibRevenge::LibRevenge
    LibRevenge::LibRevengeStream
    ${LIBWPD_LIBRARIES}
    ${LIBWPG_LIBRARIES}
)

install(TARGETS calligra_filter_wpd2odt DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

if(SHOULD_BUILD_OKULAR_GENERATOR_WORDPERFECT)
    kcoreaddons_add_plugin(okularGenerator_wpd_calligra
        INSTALL_NAMESPACE "okular/generators"
        SOURCES OkularWpdGeneratorPlugin.cpp
    )

    target_link_libraries( okularGenerator_wpd_calligra
        kookularGenerator_odt
    )

    install( FILES okularWpd_calligra.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )
    install( PROGRAMS okularApplication_wpd_calligra.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
endif()
