#if(SHOULD_BUILD_FILTER_ODT_TO_WIKI)

include_directories(
    ${CMAKE_SOURCE_DIR}/filters/libodfreader
)

set(odt2wiki_PART_SRCS
    WikiExport.cpp
    OdtReaderWikiBackend.cpp
    OdfReaderWikiContext.cpp
)
kde4_add_plugin(calligra_filter_odt2wiki ${odt2wiki_PART_SRCS})
target_link_libraries(calligra_filter_odt2wiki koodfreader komain koodf)
install(TARGETS calligra_filter_odt2wiki DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES words_wiki_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})

#endif(SHOULD_BUILD_FILTER_ODT_TO_WIKI)
