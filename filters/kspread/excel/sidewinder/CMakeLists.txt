include_directories( ${CMAKE_BINARY_DIR}/filters/ )

set(sidewinder_SRCS
  cell.cpp
  excel.cpp
  format.cpp
  pole.cpp
  sheet.cpp
  ustring.cpp
  value.cpp
  workbook.cpp

  substreamhandler.cpp
  globalssubstreamhandler.cpp
)

add_definitions(-DSWINDER_XLS2RAW)

kde4_add_executable(sidewinder TEST ${sidewinder_SRCS})
