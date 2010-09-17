include_directories(
    ${ZLIB_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder
    ${CMAKE_BINARY_DIR}/filters/
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/filters/kspread/xlsx
    ${CMAKE_SOURCE_DIR}/filters/libmsooxml
    ${CMAKE_SOURCE_DIR}/filters/libmso
    ${CMAKE_SOURCE_DIR}/kspread # for PointStorage
)

add_custom_command(
    OUTPUT records.cpp
    COMMAND recordsxml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/records.xml
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/records.xml recordsxml2cpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM)

########### next target ###############

set(sidewinder_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/cell.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/excel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/format.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/pole.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/sheet.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/value.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/workbook.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/formulas.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/utils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/objects.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/decrypt.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/substreamhandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/globalssubstreamhandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/worksheetsubstreamhandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/chartsubstreamhandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/XlsRecordOutputStream.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../xlsx/NumberFormatParser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../xlsx/ChartExport.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/records.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

set(excelimport_PART_SRCS excelimporttoods.cc ImportUtils.cpp ODrawClient.cpp ${sidewinder_SRCS})

kde4_add_plugin(excelimport WITH_PREFIX ${excelimport_PART_SRCS})


target_link_libraries(excelimport msooxml komain mso koodf ${ZLIB_LIBRARIES})
if(QCA2_FOUND)
   target_link_libraries(excelimport ${QCA2_LIBRARIES})
endif(QCA2_FOUND)

#install(TARGETS excelimport  DESTINATION ${PLUGIN_INSTALL_DIR})

set(excelimport_SRCS ExcelImport.cpp ImportUtils.cpp ODrawClient.cpp ${sidewinder_SRCS})
kde4_add_plugin(excelimporttodoc WITH_PREFIX ${excelimport_SRCS})
target_link_libraries(excelimporttodoc kspreadodf msooxml komain mso koodf ${ZLIB_LIBRARIES})
if(QCA2_FOUND)
   target_link_libraries(excelimporttodoc ${QCA2_LIBRARIES})
endif(QCA2_FOUND)

install(TARGETS excelimporttodoc DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kspread_excel_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


