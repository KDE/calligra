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

add_library(calligra_filter_odt2epub2 MODULE ${odt2epub2_PART_SRCS})

target_link_libraries(calligra_filter_odt2epub2 kovectorimage komain Qt5::Svg KF5::KDELibs4Support)

install(TARGETS calligra_filter_odt2epub2  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
kcoreaddons_desktop_to_json(calligra_filter_odt2epub2 calligra_filter_odt2epub2.desktop)

endif()


if(SHOULD_BUILD_FILTER_ODT_TO_HTML)

set(odt2html_PART_SRCS
    exporthtml.cpp
    OdfParser.cpp
    OdtHtmlConverter.cpp

    HtmlFile.cpp
    FileCollector.cpp
)

add_library(calligra_filter_odt2html MODULE ${odt2html_PART_SRCS})

target_link_libraries(calligra_filter_odt2html kovectorimage komain Qt5::Svg KF5::KDELibs4Support)

install(TARGETS calligra_filter_odt2html  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
kcoreaddons_desktop_to_json(calligra_filter_odt2html calligra_filter_odt2html.desktop)

endif()

