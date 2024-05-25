include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder
    ${CMAKE_BINARY_DIR}/filters/
    ${KOMAIN_INCLUDES}
    ${KOODF2_INCLUDES}
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx
    ${CMAKE_SOURCE_DIR}/filters/libmso
    ${CMAKE_SOURCE_DIR}/filters/libmsooxml
    ${CMAKE_SOURCE_DIR}/sheets # for PointStorage
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
  ${CMAKE_CURRENT_BINARY_DIR}/records.cpp
)

kde_enable_exceptions()

set(excelimport_PART_SRCS excelimporttoods.cc ImportUtils.cpp ODrawClient.cpp ${sidewinder_SRCS})

add_library(excelimport MODULE ${excelimport_PART_SRCS})


target_link_libraries(excelimport calligrasheetscore komsooxml komain mso koodf)

#install(TARGETS excelimport  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

set(xls2ods_SRCS ExcelImport.cpp ImportUtils.cpp ODrawClient.cpp ${sidewinder_SRCS})
add_library(calligra_filter_xls2ods MODULE ${xls2ods_SRCS})
target_link_libraries(calligra_filter_xls2ods
                      calligrasheetscore
                      komsooxml mso
                      komain koodf
)

install(TARGETS calligra_filter_xls2ods DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)


