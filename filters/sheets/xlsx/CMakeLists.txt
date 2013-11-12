add_definitions(-DKDE_DEFAULT_DEBUG_AREA=30527)

include_directories( ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/filters/libmsooxml )

########### next target ###############

set(xlsx2ods_PART_SRCS
    XlsxImport.cpp
    XlsxXmlCommonReader.cpp
    XlsxXmlDocumentReader.cpp
    XlsxXmlWorksheetReader.cpp
    XlsxXmlSharedStringsReader.cpp
    XlsxXmlStylesReader.cpp
    XlsxXmlDrawingReader.cpp
    XlsxXmlChartReader.cpp
    XlsxXmlCommentsReader.cpp
    XlsxXmlTableReader.cpp
    NumberFormatParser.cpp
    ChartExport.cpp
    FormulaParser.cpp
)

kde4_add_plugin(calligra_filter_xlsx2ods ${xlsx2ods_PART_SRCS})

target_link_libraries(
    calligra_filter_xlsx2ods
    komsooxml
    koodf
    komain
    calligrasheetscommon
)

install(TARGETS calligra_filter_xlsx2ods DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES
  calligra_filter_xlsx2ods.desktop
#  calligra_filter_ods2xlsx.desktop
  DESTINATION ${SERVICES_INSTALL_DIR}
)

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install( FILES sheets_xlsx_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)

########## unit tests ###################

set(TestFormulaParser_SRCS
    FormulaParser.cpp
    TestFormulaParser.cpp
)

kde4_add_unit_test(TestFormulaParser TESTNAME sheets-xlsx-FormulaParser ${TestFormulaParser_SRCS})
target_link_libraries(TestFormulaParser komsooxml calligrasheetscommon ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY})
