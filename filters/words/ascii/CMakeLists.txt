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
kcoreaddons_desktop_to_json(calligra_filter_ascii2words calligra_filter_ascii2words.desktop)
target_link_libraries(calligra_filter_ascii2words komain kotextlayout wordsprivate KF5::Codecs)
install(TARGETS calligra_filter_ascii2words DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

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
install(TARGETS calligra_filter_odt2ascii DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

kcoreaddons_desktop_to_json(calligra_filter_odt2ascii calligra_filter_odt2ascii.desktop)

install( PROGRAMS  calligrawords_ascii.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})

endif()
