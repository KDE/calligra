include_directories(
    ${CMAKE_BINARY_DIR}/filters/
    ${CMAKE_SOURCE_DIR}/filters/libmsooxml  # for MsooXmlGlobal.h
    ${CMAKE_SOURCE_DIR}/filters/libmso
    ${KOODF2_INCLUDES}                      # For the chart stuff
    ${CMAKE_SOURCE_DIR}                     # for DataFilter.h and PointStorage.h
    ${KOTEXT_INCLUDES}                      # for KoTextRangeManager.h, IMHO could be removed again
    ${KOODF_INCLUDES}
)

add_executable(recordsxml2cpp recordsxml2cpp.cpp)
target_link_libraries(recordsxml2cpp Qt6::Core Qt6::Xml)

add_custom_command(
    OUTPUT records.cpp
    COMMAND recordsxml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/records.xml
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/records.xml recordsxml2cpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM)

set(sidewinder_SRCS
  cell.cpp
  excel.cpp
  format.cpp
  sheet.cpp
  value.cpp
  workbook.cpp
  formulas.cpp
  utils.cpp
  objects.cpp
  decrypt.cpp
  conditionals.cpp
  substreamhandler.cpp
  globalssubstreamhandler.cpp
  worksheetsubstreamhandler.cpp
  chartsubstreamhandler.cpp
  XlsRecordOutputStream.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/records.cpp
)

kde_enable_exceptions()

add_definitions(-DSWINDER_XLS2RAW)

add_executable(sidewinder ${sidewinder_SRCS})

target_link_libraries(sidewinder mso calligrasheetscore Qt6::Core Qt6::Gui koodf)
