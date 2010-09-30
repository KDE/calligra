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
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/conditionals.cpp
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

set(excelexport_SRCS ExcelExport.cpp CFBWriter.cpp ${sidewinder_SRCS})

kde4_add_plugin(excelexport WITH_PREFIX ${excelexport_SRCS})


target_link_libraries(excelexport msooxml komain mso koodf ${ZLIB_LIBRARIES} kspreadcommon)
if(QCA2_FOUND) # because sidewinder depends on it
   target_link_libraries(excelexport ${QCA2_LIBRARIES})
endif(QCA2_FOUND)

set(INSTALL_XLS_EXPORT_FILTER OFF CACHE BOOL "Install the very experimental xls export filter, which might eat your data and provides no guarentees")

if(INSTALL_XLS_EXPORT_FILTER)
  install(TARGETS excelexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

  install( FILES  kspread_excel_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif(INSTALL_XLS_EXPORT_FILTER)

