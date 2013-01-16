########### next target ###############

include_directories(
    ${VECTORIMAGE_INCLUDES}
    ${KOMAIN_INCLUDES}
    ${FLAKE_INCLUDES}
)
set(exportepub2_PART_SRCS
    exportepub2.cpp
    OdfParser.cpp
    OdtHtmlConverter.cpp

    EpubFile.cpp
    FileCollector.cpp
)

set(exporthtml_PART_SRCS
    exporthtml.cpp
    OdfParser.cpp
    OdtHtmlConverter.cpp

    HtmlFile.cpp
    FileCollector.cpp
)


kde4_add_plugin(exportepub2 ${exportepub2_PART_SRCS})
kde4_add_plugin(exporthtml ${exporthtml_PART_SRCS})

target_link_libraries(exportepub2 vectorimage komain)
target_link_libraries(exporthtml vectorimage komain)


 ########### install files ###############

install(TARGETS exportepub2  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  words_epub2_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

install(TARGETS exporthtml  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  words_html_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
