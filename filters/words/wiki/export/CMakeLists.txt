#if(SHOULD_BUILD_FILTER_ODT_TO_WIKI)

include_directories(
    ${KOODFREADER_INCLUDES}
    ${KOODF2_INCLUDES}
)

set(odt2wiki_SRCS
    WikiExport.cpp
    OdtReaderWikiBackend.cpp
    OdfReaderWikiContext.cpp
)
kde4_add_plugin(calligra_filter_odt2wiki ${odt2wiki_SRCS})
target_link_libraries(calligra_filter_odt2wiki koodfreader komain koodf2)

install(TARGETS calligra_filter_odt2wiki DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES calligra_filter_odt2wiki.desktop DESTINATION ${SERVICES_INSTALL_DIR})

#endif(SHOULD_BUILD_FILTER_ODT_TO_WIKI)
