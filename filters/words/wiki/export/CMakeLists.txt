
include_directories(
    ${CMAKE_SOURCE_DIR}/filters/libodfreader
    ${CMAKE_SOURCE_DIR}/filters/libodf2
)

set(odt2wiki_PART_SRCS
    WikiExportDebug.cpp
    WikiExport.cpp
    OdtReaderWikiBackend.cpp
    OdfReaderWikiContext.cpp
)
add_library(calligra_filter_odt2wiki MODULE ${odt2wiki_PART_SRCS})
target_link_libraries(calligra_filter_odt2wiki koodfreader komain koodf2)

install(TARGETS calligra_filter_odt2wiki DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)
