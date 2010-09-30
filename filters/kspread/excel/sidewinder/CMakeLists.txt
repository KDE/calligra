include_directories(
    ${CMAKE_BINARY_DIR}/libs/store
    ${CMAKE_BINARY_DIR}/filters/
    ${CMAKE_SOURCE_DIR}/filters/libmso
    ${CMAKE_SOURCE_DIR}/filters/kspread/xlsx # for Charting.h
    ${CMAKE_SOURCE_DIR}/filters/libmsooxml  # for MsooXmlGlobal.h
    ${CMAKE_SOURCE_DIR}/kspread # for PointStorage.h
)

kde4_add_executable(recordsxml2cpp recordsxml2cpp.cpp)
target_link_libraries(recordsxml2cpp ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY})

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
  pole.cpp
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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

add_definitions(-DSWINDER_XLS2RAW)

kde4_add_executable(sidewinder TEST ${sidewinder_SRCS})

target_link_libraries(sidewinder ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${ZLIB_LIBRARIES} mso koodf)
if(QCA2_FOUND)
   target_link_libraries(sidewinder ${QCA2_LIBRARIES})
endif(QCA2_FOUND)
