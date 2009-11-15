include_directories( ${CMAKE_BINARY_DIR}/filters/ )


kde4_add_executable(recordsxml2cpp recordsxml2cpp.cpp)
target_link_libraries(recordsxml2cpp ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY})

add_custom_command(
    OUTPUT records.cpp
    COMMAND recordsxml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/records.xml
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/records.xml ${CMAKE_CURRENT_BINARY_DIR}/recordsxml2cpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM)

set(sidewinder_SRCS
  cell.cpp
  excel.cpp
  format.cpp
  pole.cpp
  sheet.cpp
  ustring.cpp
  value.cpp
  workbook.cpp
  formulas.cpp
  utils.cpp
  substreamhandler.cpp
  globalssubstreamhandler.cpp
  worksheetsubstreamhandler.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/records.cpp
)

add_definitions(-DSWINDER_XLS2RAW)

kde4_add_executable(sidewinder TEST ${sidewinder_SRCS})
target_link_libraries(sidewinder ${QT_QTCORE_LIBRARY})


