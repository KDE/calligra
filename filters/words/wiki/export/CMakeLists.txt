#if(SHOULD_BUILD_FILTER_ODT_TO_WIKI)

include_directories(
    ${CMAKE_SOURCE_DIR}/filters/libodfreader
    ${CMAKE_SOURCE_DIR}/filters/libodf2
)

set(odt2wiki_PART_SRCS
    WikiExport.cpp
    OdtReaderWikiBackend.cpp
    OdfReaderWikiContext.cpp
    
    ${CMAKE_SOURCE_DIR}/filters/libodf2/KoOdfStyleManager.cpp
    ${CMAKE_SOURCE_DIR}/filters/libodf2/KoOdfStyleProperties.cpp
    ${CMAKE_SOURCE_DIR}/filters/libodf2/KoOdfStyle.cpp
    ${CMAKE_SOURCE_DIR}/filters/libodf2/KoOdfTextProperties.cpp
    ${CMAKE_SOURCE_DIR}/filters/libodf2/KoOdfParagraphProperties.cpp
    ${CMAKE_SOURCE_DIR}/filters/libodf2/KoOdfGraphicProperties.cpp
)
kde4_add_plugin(calligra_filter_odt2wiki ${odt2wiki_PART_SRCS})
target_link_libraries(calligra_filter_odt2wiki koodfreader komain koodf)
install(TARGETS calligra_filter_odt2wiki DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES words_wiki_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})

#endif(SHOULD_BUILD_FILTER_ODT_TO_WIKI)
