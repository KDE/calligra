include_directories(
    ${VECTORIMAGE_INCLUDES}
    ${KOMAIN_INCLUDES}
    ${FLAKE_INCLUDES}
)


if(SHOULD_BUILD_FILTER_ODT_TO_EPUB2)

set(odt2epub2_PART_SRCS
    exportepub2.cpp
    OdfParser.cpp
    OdtHtmlConverter.cpp

    EpubFile.cpp
    FileCollector.cpp
)

kde4_add_plugin(calligra_filter_odt2epub2 ${odt2epub2_PART_SRCS})

target_link_libraries(calligra_filter_odt2epub2 kovectorimage komain)

install(TARGETS calligra_filter_odt2epub2  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  calligra_filter_odt2epub2.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_ODT_TO_EPUB2)


if(SHOULD_BUILD_FILTER_ODT_TO_HTML)

set(odt2html_PART_SRCS
    exporthtml.cpp
    OdfParser.cpp
    OdtHtmlConverter.cpp

    HtmlFile.cpp
    FileCollector.cpp
)

kde4_add_plugin(calligra_filter_odt2html ${odt2html_PART_SRCS})

target_link_libraries(calligra_filter_odt2html kovectorimage komain)

install(TARGETS calligra_filter_odt2html  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  calligra_filter_odt2html.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_ODT_TO_HTML)

