if(SHOULD_BUILD_FILTER_ASCII_TO_WORDS)

include_directories(
    ${CMAKE_SOURCE_DIR}/words/part
)


set(ascii2words_PART_SRCS
    AsciiImportDebug.cpp
    AsciiImport.cpp
    ImportDialog.cc
)
ki18n_wrap_ui(ascii2words_PART_SRCS ImportDialogUI.ui )
add_library(calligra_filter_ascii2words MODULE ${ascii2words_PART_SRCS})
target_link_libraries(calligra_filter_ascii2words komain kotextlayout wordsprivate)
install(TARGETS calligra_filter_ascii2words DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

endif()


if(SHOULD_BUILD_FILTER_ODT_TO_ASCII)

include_directories(
    ${CMAKE_SOURCE_DIR}/filters/libodfreader
    ${KOODF2_INCLUDES}
)

set(odt2ascii_PART_SRCS
    AsciiExportDebug.cpp
    AsciiExport.cpp
    OdtReaderAsciiBackend.cpp
    OdfReaderAsciiContext.cpp
)
add_library(calligra_filter_odt2ascii MODULE ${odt2ascii_PART_SRCS})
target_link_libraries(calligra_filter_odt2ascii koodfreader komain koodf koodf2)
install(TARGETS calligra_filter_odt2ascii DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)


install( PROGRAMS  org.kde.calligrawords_ascii.desktop  DESTINATION ${KDE_INSTALL_APPDIR})

endif()
