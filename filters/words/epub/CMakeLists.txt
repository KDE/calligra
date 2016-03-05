include_directories(
    ${VECTORIMAGE_INCLUDES}
    ${KOMAIN_INCLUDES}
    ${FLAKE_INCLUDES}
)


if(SHOULD_BUILD_FILTER_ODT_TO_EPUB2)

set(odt2epub2_PART_SRCS
    EpubExportDebug.cpp
    exportepub2.cpp
    OdfParser.cpp
    OdtHtmlConverter.cpp

    EpubFile.cpp
    FileCollector.cpp
)

add_library(calligra_filter_odt2epub2 MODULE ${odt2epub2_PART_SRCS})
set_target_properties(calligra_filter_odt2epub2 PROPERTIES COMPILE_DEFINITIONS "DEBUG_EPUB=1")

target_link_libraries(calligra_filter_odt2epub2 kovectorimage komain Qt5::Svg)

install(TARGETS calligra_filter_odt2epub2  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)
kcoreaddons_desktop_to_json(calligra_filter_odt2epub2 calligra_filter_odt2epub2.desktop)

endif()


if(SHOULD_BUILD_FILTER_ODT_TO_HTML)

set(odt2html_PART_SRCS
    HtmlExportDebug.cpp
    exporthtml.cpp
    OdfParser.cpp
    OdtHtmlConverter.cpp

    HtmlFile.cpp
    FileCollector.cpp
)

add_library(calligra_filter_odt2html MODULE ${odt2html_PART_SRCS})
set_target_properties(calligra_filter_odt2html PROPERTIES COMPILE_DEFINITIONS "DEBUG_HTML=1")

target_link_libraries(calligra_filter_odt2html kovectorimage komain Qt5::Svg)

install(TARGETS calligra_filter_odt2html  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)
kcoreaddons_desktop_to_json(calligra_filter_odt2html calligra_filter_odt2html.desktop)

endif()

