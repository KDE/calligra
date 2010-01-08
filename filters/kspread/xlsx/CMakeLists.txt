add_definitions(-DKDE_DEFAULT_DEBUG_AREA=30527)

include_directories( ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/filters/libmsooxml )

########### next target ###############

set(xlsximport_PART_SRCS
    ColorConversions.cpp # TODO: only include for TINY target
    XlsxImport.cpp
    XlsxXmlCommonReader.cpp
    XlsxXmlDocumentReader.cpp
    XlsxXmlWorksheetReader.cpp
    XlsxXmlSharedStringsReader.cpp
    XlsxXmlStylesReader.cpp
    NumberFormatParser.cpp
)

kde4_add_plugin(xlsximport WITH_PREFIX ${xlsximport_PART_SRCS})

target_link_libraries(
    xlsximport
    msooxml
    koodf
    komain
    kspreadcommon
)

install(TARGETS xlsximport DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES 
  kspread_xlsx_import.desktop
#  kspread_xlsx_export.desktop
  DESTINATION ${SERVICES_INSTALL_DIR}
)
