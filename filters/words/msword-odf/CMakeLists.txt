# FIXME Build wv2 on windows
if(NOT WIN32)
    add_subdirectory(wv2)
endif()

include_directories(
    ${ICONV_INCLUDE_DIR}
    ../../libmso
)

set(doc2odt_PART_SRCS
   MsDocDebug.cpp
   mswordodfimport.cpp
   document.cpp
   conversion.cpp
   texthandler.cpp
   paragraph.cpp
   tablehandler.cpp
   graphicshandler.cpp
   drawclient.cpp
)

kde_enable_exceptions()

add_library(calligra_filter_doc2odt MODULE ${doc2odt_PART_SRCS})

target_link_libraries(calligra_filter_doc2odt mso kowv2 komain)

install(TARGETS calligra_filter_doc2odt  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

if(SHOULD_BUILD_OKULAR_GENERATOR_DOC)
    kcoreaddons_add_plugin(okularGenerator_doc_calligra
        INSTALL_NAMESPACE "okular_generators"
        SOURCES OkularDocGeneratorPlugin.cpp
    )

    target_link_libraries( okularGenerator_doc_calligra
        kookularGenerator_odt
    )
    install( PROGRAMS okularApplication_doc_calligra.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
endif()
