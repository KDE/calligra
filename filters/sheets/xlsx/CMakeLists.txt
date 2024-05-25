add_definitions(-DKDE_DEFAULT_DEBUG_AREA=30527)

include_directories(${KOMAIN_INCLUDES}
                    ${KOODF2_INCLUDES}           # For KoOdfChartWriter
                    ${CMAKE_SOURCE_DIR}/filters/libmso
                    ${CMAKE_SOURCE_DIR}/filters/libmsooxml
                    ${CMAKE_SOURCE_DIR}/sheets
)

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

    XlsxChartOdfWriter.cpp
    FormulaParser.cpp
)

add_library(calligra_filter_xlsx2ods MODULE ${xlsx2ods_PART_SRCS})

target_link_libraries(
    calligra_filter_xlsx2ods
    koodf2
    komsooxml
    mso
    koodf
    komain
    calligrasheetscore
)

install(TARGETS calligra_filter_xlsx2ods DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

########## unit tests ###################

set(TestFormulaParser_SRCS
    FormulaParser.cpp
    TestFormulaParser.cpp
)

ecm_add_test( ${TestFormulaParser_SRCS}
    TEST_NAME "FormulaParser"
    NAME_PREFIX "filter-xlsx2ods-"
    LINK_LIBRARIES komsooxml calligrasheetsui Qt6::Test
)
